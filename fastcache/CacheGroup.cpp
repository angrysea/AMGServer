#include "CacheGroup.h"
#include "Item.h"
#include "DataArray.h"

CacheGroup::CacheGroup(void) 
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

CacheGroup::~CacheGroup(void)
{
	/*
	ADVISESINKVECTOR::iterator it = m_adviseSinkVector.begin();  
 
	for(AdviseSink * sink : m_adviseSinkVector)
	{       
		delete sink;
	} 

	for(auto pair : m_ItemMap)
    {         
		delete pair.second;
    } 
	*/
}

long CacheGroup::Init()
{
	return RET_OK;
}

unsigned long
CacheGroup::itemCount()
{
	return m_bActive == true ? m_ItemMap.size() : 0 ;
}

long CacheGroup::setGroupItemData(DATAITEM * pvarGuids, DATAITEM * pvarValues, DATAITEM * pvarQualities)
{
	long hr = RET_OK;

	DATAARRAY * psaGuids = 0;
	DATAARRAY * psaValues = 0;
	DATAARRAY * psaQualities = 0;

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

	unsigned long nElements = 0;
	DataArrayGetSize(psaGuids, &nElements);

	DATAITEM * pvarGuidResponse;
	DATAITEM  * pvarValueResponse;
	DATAITEM* pvarQualityResponse;

	DataArrayAccessData(psaGuids, (void**)&pvarGuidResponse);
	if (pvarGuidResponse==0)
	{
		return RET_FAIL;
	}
	
	DataArrayAccessData(psaValues, (void**)&pvarValueResponse);
	if (pvarValueResponse==0)
	{	
	   return RET_FAIL;
	}
   
	DataArrayAccessData(psaQualities, (void**)&pvarQualityResponse);
	if (pvarQualityResponse==0)
	{
		return RET_FAIL;
	}

	ITEMMAP::iterator itemIterator;
	for (unsigned long i=0; i<nElements; i++)
	{
		char * bstrGuid = pvarGuidResponse[i].sstringVal;
		for(auto pair : m_ItemMap)
		{
			Item * pItem = pair.second;
			if (strcmp(pItem->getGuid(), bstrGuid) == 0)
			{
	            // Found the item!
				pItem->setQuality((pvarQualityResponse[i].lVal)==1?0xC0:0x00);
				hr = pItem->SetValue(&pvarValueResponse[i]); // m_hServerGroup  m_hClientGroup
				break;
			}
			advance(itemIterator, 1);
		}
	}

	DataArrayUnaccessData(psaGuids);
	DataArrayUnaccessData(psaValues);
	DataArrayUnaccessData(psaQualities);
	
	return hr;
}

long CacheGroup::advanceTime(unsigned long deltaTime, unsigned long* pCount, Item** & ppItems )
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
		if (e==RET_FAIL)
		{
			if (ppItems != 0)
			{
	            delete [] ppItems;
				ppItems = 0;
			}
			*pCount = 0;
         hr		= e;
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
CacheGroup::notifySinks(unsigned long deltaTime)
{  
	long hr = RET_OK;
	bool bDirty = false;
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
   		unsigned long * itemHandleList = new unsigned long[ m_ItemMap.size() ];            
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
					/*
					CThread * pThread = new CThread( true );
					pThread->setRunnable( new CSendStream( this,
											nCount, 
                                            itemHandleList, 
                                            (*theIterator), 
                                            &m_SSCriticalSection, 
                                            0, //transactionid*
                                            OPC_DS_CACHE ) );                     
					pThread->start();
				*/
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
		hr = RET_OUTOFMEMORY;
		// "Bad Alloc exception caught in CacheGroup::AdvanceTime\n";
	}
	catch( ... )
	{
		hr = RET_UNEXPECTED;
		"Unexpected error caught in CacheGroup::AdvanceTime\n";
	}

   //LeaveCriticalSection(&m_CriticalSection);

	return RET_OK;
}

long CacheGroup::ReadData()
{
	long hr = RET_OK;
	unsigned long itemIndex = 0;

	Item** ppItems = 0;
	unsigned long itemCount = 0;
	unsigned long nCount = 0;

	try
	{
		itemCount = m_ItemMap.size();

		ITEMMAP::iterator theIterator;

		if(itemCount > 0)
		{
			ppItems = new Item*[itemCount];
			theIterator = m_ItemMap.begin();

			Item * pItem = 0;
			while(theIterator != m_ItemMap.end())
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

		delete [] ppItems;
	}
	catch (long e)
	{
		if (RET_FAIL == e )
		{
			//"Receive Query for GetDataItemValues Threw exception \n";
		}
		hr = e;
	}
	catch (...)
	{
		//"Receive Query for GetDataItemValues Threw exception \n";
		hr = RET_FAIL;
	}

	return hr;
}