#include "RTGroup.h"
#include "Item.h"
#include "SafeArray.h"


RTGroup::RTGroup(void) 
	:m_hServerGroup(0),
	m_hClientGroup(0),
	m_ulRevisedRate(0),
	m_bActive(false),
    m_Deadband(0),
    m_hNext(0),
    m_dwDataCallback(0),
	m_dwDatatimeCallback(0),
	m_dwWriteCallback(0),
    //m_adviseSinkVector(0),
    m_ulAccumTime(99999)
{   
}


RTGroup::~RTGroup(void)
{
   ADVISESINKVECTOR::iterator it = m_adviseSinkVector.begin();  
 
   for each(AdviseSink * sink in m_adviseSinkVector)
   {       
      delete sink;
   } 

	for each(auto & pair in m_ItemMap)
    {         
		delete pair.second;
    } 
}

long RTGroup::Init()
{
   return RET_OK;
}


unsigned long
RTGroup::itemCount()
{
   return m_bActive == true ? m_ItemMap.size() : 0 ;
}

long RTGroup::setGroupItemData(DATAITEM* pvarGuids, DATAITEM* pvarValues, DATAITEM* pvarQualities)
{
   long hr = RET_OK;

   SAFEARRAY * psaGuids = 0;
   SAFEARRAY * psaValues = 0;
   SAFEARRAY * psaQualities = 0;
   long uBound, lBound, nElements;
   int i;

   if (DT_DT(pvarGuids) != (DT_ARRAY | DT_DATAITEM) ||
       DT_DT(pvarValues) != (DT_ARRAY | DT_DATAITEM) ||
       DT_DT(pvarQualities) != (DT_ARRAY | DT_DATAITEM))
   {
      // One of more of the incoming variants are of the wrong type.
      return RET_FAIL;
   }

   psaGuids = DT_ARRAY(pvarGuids);
   psaValues = DT_ARRAY(pvarValues);
   psaQualities = DT_ARRAY(pvarQualities);
   
   nElements = SafeArrayGetDim(psaGuids);

   DATAITEM * pvarGuidResponse;
   DATAITEM  * pvarValueResponse;
   DATAITEM* pvarQualityResponse;

   SafeArrayAccessData(psaGuids, (void**)&pvarGuidResponse);
   SafeArrayAccessData(psaValues, (void**)&pvarValueResponse);
   SafeArrayAccessData(psaQualities, (void**)&pvarQualityResponse);

   ITEMMAP::iterator itemIterator;
   for (i=0; i<nElements; i++)
   {
      wchar_t * bstrGuid = pvarGuidResponse[i].sstringVal;
		for each(auto & pair in m_ItemMap)
      {
         Item * pItem = pair.second;
         if (wcscmp(pItem->getGuid(), bstrGuid) == 0)
         {
            // Found the item!
            pItem->setQuality((pvarQualityResponse[i].lVal)==1?0xC0:0x00);
            hr = pItem->SetValue(pvarValueResponse[i]); // m_hServerGroup  m_hClientGroup
            break;
         }
         advance(itemIterator, 1);
      }
   }

   SafeArrayUnaccessData(psaGuids);
   SafeArrayUnaccessData(psaValues);
   SafeArrayUnaccessData(psaQualities);

   return hr;
}

long
RTGroup::advanceTime(unsigned long deltaTime, unsigned long* pCount, Item** & ppItems )
{
   // Build a list of all Items in this group that are active and
   // need to have current data retreived from .
   long hr;
   ITEMMAP::iterator theIterator;
   *pCount = 0;
   //*ppItems = 0;

   // There are no items in this group, or the group is currently inactive.
   if (m_ItemMap.size() <= 0 || m_bActive == false)
   {
      return RET_OK;
   }

   m_ulAccumTime += deltaTime;
   if (m_ulAccumTime < m_ulRevisedRate)
   {
      // Not time yet.
      return RET_OK;
   }

   //EnterCriticalSection(&m_CriticalSection);

   try
   {
      m_ulAccumTime = 0;
      
      int nCount = itemCount();
      
      if ( nCount > 0 )
      {
         ppItems = new Item*[nCount];
         theIterator = m_ItemMap.begin();
         
         Item * pItem = 0;
         while (theIterator != m_ItemMap.end() )
         {
            pItem = (*theIterator).second;
            bool bActive;
            pItem->GetActive(&bActive);
            if (bActive)
            {
               (*pCount)++;
               ppItems[*pCount-1] = pItem;
            }
            advance(theIterator, 1);
         }
      }
   }
   catch ( long e )
   {
      if (e==RET_FAIL) != 0 )
      {
         if (ppItems != 0)
         {
            delete [] ppItems;
            ppItems = 0;
         }
         *pCount = 0;
         hr = e;
      }
   }
   catch (...)
   {
      if (ppItems != 0)
      {
         delete [] ppItems;
         ppItems = 0;
      }
      *pCount = 0;
      hr = RET_FAIL;
   }
   
   //LeaveCriticalSection(&m_CriticalSection);

   return hr;
}

long 
RTGroup::notifySinks(unsigned long deltaTime)
{  
   long     hr     = RET_OK;
   bool        bDirty = false;
   Item *  pItem  = 0;
   ITEMMAP::iterator theIterator;
   
   if (m_ItemMap.size() <= 0 || m_bActive == false )
   {
      return RET_OK;
   }

   m_ulAccumTime += deltaTime;
   if (m_ulAccumTime < m_ulRevisedRate)
   {
      // Not time yet.
      return RET_OK;
   }

   m_ulAccumTime = 0;

   //EnterCriticalSection(&m_CriticalSection);

   try
   {
      int nCount = 0;                   
   
      long * itemHandleList = new long[ m_ItemMap.size() ];            

      ITEMMAP::iterator theIterator;

      theIterator = m_ItemMap.begin();     

      while ( theIterator != m_ItemMap.end() )
      {          
         pItem = (*theIterator).second;

         long hr = RET_OK;
    
         hr = pItem->IsDirty( &bDirty );
         if( (hr==RET_OK  && bDirty ) || ( m_Deadband == 0 ) )                  
         {
            itemHandleList[nCount] = 0;
            if (hr=pItem->GetServerHandle( &itemHandleList[nCount])==RET_OK)
            {
               nCount++;
            }
         }
         advance( theIterator, 1 );
      }		      
      
	   if( nCount > 0 )
	   {  
         ADVISESINKVECTOR::iterator theIterator;
         for ( theIterator = m_adviseSinkVector.begin(); 
               theIterator != m_adviseSinkVector.end();
               theIterator++  )    
         {                    
            if ( (*theIterator) != 0 )
            {
               CThread * pThread = new CThread( true );
               pThread->setRunnable( new CSendStream( this,
                                                      nCount, 
                                                      itemHandleList, 
                                                      (*theIterator), 
                                                      &m_SSCriticalSection, 
                                                      0/*transactionid*/, 
                                                      OPC_DS_CACHE ) );                     
               pThread->start();
            }
         }
	   }            
      else
      {
         delete []itemHandleList;
      }
   }
   catch( bad_alloc )
   {
      hr = E_OUTOFMEMORY;
      ATLTRACE( _T("Bad Alloc exception caught in RTGroup::AdvanceTime\n") );
   }
   catch( ... )
   {
      hr = E_UNEXPECTED;
      ATLTRACE( _T("Unexpected error caught in RTGroup::AdvanceTime\n") );
   }

   //LeaveCriticalSection(&m_CriticalSection);

   return RET_OK;
}

long RTGroup::ReadData()
{
   long hr = RET_OK;
   long i;
   unsigned long itemIndex = 0;

   Item** ppItems = 0;
   unsigned long itemCount = 0;
	unsigned long nCount = 0;

	try
	{
      itemCount = m_ItemMap.size();

      ITEMMAP::iterator theIterator;

		if ( itemCount > 0 )
		{
		   ppItems = new Item*[itemCount];
		   theIterator = m_ItemMap.begin();

		   Item * pItem = 0;
		   while (theIterator != m_ItemMap.end() )
		   {
			   pItem = (*theIterator).second;
			   bool bActive;
			   pItem->GetActive(&bActive);
			   if (bActive)
			   {
			      ppItems[nCount] = pItem;
			      nCount++;
			   }
			   advance(theIterator, 1);
		   }
	   }

      if ( nCount <= 0 )
      {
         delete [] ppItems;
         throw RET_FALSE;
      }

      itemCount = nCount;

      DISPPARAMS dispparams;
      dispparams.cNamedArgs = 0;
      dispparams.cArgs = 1;
      dispparams.rgvarg = new DATAITEM;
      
      DT_DT(&dispparams.rgvarg[0]) = DT_ARRAY | DT_DATAITEM;
      SAFEARRAY* psaItemID = SafeArrayCreateVector( DT_DATAITEM, 0, itemCount );
      
      if (psaItemID == NULL)
      {
         delete dispparams.rgvarg;
         throw E_OUTOFMEMORY;
      }

      DT_ARRAY(&dispparams.rgvarg[0]) = psaItemID;

      DATAITEM * pvarItemID;
      if ( FAILED( SafeArrayAccessData( psaItemID, (void **)&pvarItemID ) ) )
      {
         DataItemClear( &dispparams.rgvarg[0] );
         delete dispparams.rgvarg;
         throw RET_FAIL;
      }

      for (i = 0; i<itemCount; i++)
      {
         Item* pItem = ppItems[i];
         DT_DT(&pvarItemID[i]) = DT_wchar_t *;
         V_wchar_t *(&pvarItemID[i]) = SysAllocString(pItem->getGuid());
      }
      
      SafeArrayUnaccessData( psaItemID );

      CISPQuery query(&dispparams,              // theQuestion
                         CGuid(std::wstring(L"root")),           // Object in  at which the question is directed
                         CGuid(),                  // unique QueryID
                         L"GetDataItemValues");    // Method to be invoked on the specified object
/*
      CISPRoot::getCurrentRoot()->addCOMQuery(&query);
      query.wait();
*/
      DataItemClear( &dispparams.rgvarg[0] );
      delete dispparams.rgvarg;

      if ( (hr = query.getHR()) == RET_OK)
      {
         // Get the answer back from the query; this should contain
         // a list for the current data for all the requested items
         // and a list for the data qualities (in parallel).
         DATAITEM varAnswer;
         DataItemInit(&varAnswer);
         query.getAnswer(varAnswer);

         if (DT_DT(&varAnswer) != (DT_ARRAY | DT_DATAITEM) )
         {
            // The answer variant came back as the wrong type
            DataItemClear(&varAnswer);
            delete [] ppItems;
            throw E_UNEXPECTED;
         }

         long nElements;
         SAFEARRAY* pSafeArray = DT_ARRAY(&varAnswer);
         if (SafeArrayGetDim(pSafeArray) > 1)
         {
            // Too many dimensions in this safearray
            DataItemClear(&varAnswer);
            delete [] ppItems;
            throw E_UNEXPECTED;
         }
         nElements = SafeArrayGetElemsize(pSafeArray);
         if (nElements != 2)
         {
            // We received a different # of items than we asked for!
            DataItemClear(&varAnswer);
            delete [] ppItems;
            throw E_UNEXPECTED;
         }

         DATAITEM * pvarAnswer;
         // Get the lists.
		 SafeArrayAccessData( pSafeArray, (void **)&pvarAnswer );
         if ( pvarAnswer == 0 )
         {
            DataItemClear(&varAnswer);
            delete [] ppItems;
            throw RET_FAIL;
         }
         // Get the safe array of values.
         SAFEARRAY* psaValues = DT_ARRAY(&pvarAnswer[0]);
         SAFEARRAY* psaQualities = DT_ARRAY(&pvarAnswer[1]);

         if ( psaValues == 0 || psaQualities == 0 )
         {
            SafeArrayUnaccessData( pSafeArray );
            DataItemClear(&varAnswer);
            delete [] ppItems;
            throw RET_FAIL;
         }
         nElements = SafeArrayGetElemsize(psaValues);
         // Go through each element of the answer and get the current data.
         // Handle the case where the data item doesn't exist in  anymore also.
         DATAITEM * pvarDataValueResponse;
         DATAITEM * pvarDataQualityResponse;
		 SafeArrayAccessData( psaValues, (void **)&pvarDataValueResponse );
         if (pvarDataValueResponse == 0)
         {
            SafeArrayUnaccessData( pSafeArray );
            DataItemClear(&varAnswer);
            delete [] ppItems;
            throw RET_FAIL;
         }
		 SafeArrayAccessData( psaQualities, (void **)&pvarDataQualityResponse );
         if (pvarDataQualityResponse==0)
         {
            SafeArrayUnaccessData( psaValues );
            SafeArrayUnaccessData( pSafeArray );
            DataItemClear(&varAnswer);
            delete [] ppItems;
            throw RET_FAIL;
         }

         for (i=0; i<nElements; i++)
         {
            // OPC_QUALITY_GOOD=0xC0   OPC_QUALITY_BAD=0x00   OPC_QUALITY_UNCERTAIN=0x40
            Item* pItem = ppItems[i];

            if (DT_DT(&pvarDataValueResponse[i]) == DT_NULL)
            {
               // This piece of data doesn't exist in  anymore.
               pItem->setQuality(00); // OPC_QUALITY_BAD
            }
            else
            {
               pItem->setQuality( pvarDataQualityResponse[i].lVal==1?0xC0:0x00 );
               hr = pItem->SetValue(pvarDataValueResponse[i]);
            }
         }

         SafeArrayUnaccessData( psaValues );
         SafeArrayUnaccessData( psaQualities );
         SafeArrayUnaccessData( pSafeArray );
         DataItemClear(&varAnswer);
      }

      delete [] ppItems;
   }
   catch (long e)
   {
      if ( FAILED( e ) != 0 )
      {
         // L"Receive Query for GetDataItemValues Threw exception \n"
      }
      hr = e;
   }
   catch (...)
   {
      // L"Receive Query for GetDataItemValues Threw exception \n"
      hr = RET_FAIL;
   }

   return hr;
}