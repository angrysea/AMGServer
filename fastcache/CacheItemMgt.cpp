#include <string>

#include "CacheGroup.h"
#include "DataItem.h"
#include "Item.h"
#include "DataArray.h"
#include "CopyData.h"

long  CacheGroup::AddItems(	unsigned long dwNumItems, 
                            ITEMDEF * pItemArray, 
                            ITEMRESULT ** ppAddResults, 
                            long ** ppErrors )
{   
	ITEMRESULT * pAddResults = 0;
	long * pErrors = 0;
	long hr = RET_OK;
	Item * pItem;
	bool partialSuccessFlag = false;

	if (dwNumItems <= 0)
	{
		return RET_INVALIDARG;
	}
  
	*ppAddResults = 0;
	*ppErrors = 0;

	//EnterCriticalSection(&m_CriticalSection);

	try
	{
		pAddResults = (ITEMRESULT*)malloc(sizeof(ITEMRESULT) * dwNumItems);
		if(pAddResults == 0)
		{
			throw RET_OUTOFMEMORY;
		}      
	   
		pErrors = (long*)malloc( sizeof( long ) * dwNumItems );
		if(pErrors == 0)
		{
			throw RET_OUTOFMEMORY; 
		}

		*ppAddResults = pAddResults;
		*ppErrors     = pErrors;
	}
	catch ( long e )
	{
		free(pAddResults );
		free(pErrors );      
		return e;
	}
	catch( ... )
	{
		free(pAddResults);
		free(pErrors);      
		return RET_UNEXPECTED;
	}   

	// Initialize memory
	memset( pAddResults, 0, sizeof( ITEMRESULT ) * dwNumItems );
	for(unsigned long i = 0; i < dwNumItems; i++)
	{
		pErrors[i] = RET_OK;
	}

	try
	{
		// Build a query to  to add items to the group, both the  group and the  group.
		DATAITEM * pdiCookie   = new DATAITEM;
		DATAITEM * pdiGroupID  = new DATAITEM;
		DATAITEM * pdiNumItems = new DATAITEM;
		DATAITEM * pdiItemID   = new DATAITEM;

		DataItemInit(pdiCookie);
		DataItemInit(pdiGroupID);
		DataItemInit(pdiNumItems);
		DataItemInit(pdiItemID);

		pdiCookie->dt = DT_SSTR;
		pdiGroupID->dt = DT_SSTR;
		pdiNumItems->dt = DT_I4;
		pdiItemID->dt = DT_ARRAY | DT_DATAITEM;

		pdiCookie->sstringVal = SafeString("");
		pdiGroupID->sstringVal = SafeString(m_strGroupID.data());
		pdiNumItems->lVal = dwNumItems;
		DATAARRAY * psaItemID = DataArray(DT_DATAITEM, dwNumItems);

		if (psaItemID == NULL)
		{
			return RET_OUTOFMEMORY;
		}

		pdiItemID->parray = psaItemID;

		for (unsigned long i=0; i<dwNumItems; i++)
		{
			DATAITEM* aDataItem = new DATAITEM;
			DataItemInit(aDataItem);
			aDataItem->dt = DT_SSTR;
			aDataItem->sstringVal = SafeString(pItemArray[i].szItemID);
			DataArrayPutElement(psaItemID, i, aDataItem);
			DataItemClear(aDataItem);
			delete aDataItem;
		}

		// theQuestion
		// Object in  at which the question is directed
		// unique QueryID
		// Method to be invoked

		DataItemClear(pdiCookie);
		DataItemClear(pdiGroupID);
		DataItemClear(pdiNumItems);
		DataItemClear(pdiItemID);

		// Get the answer back from the query. The answer should be of the following format:
		// arg1: List of <strings> of Guids representing  data objects. Zero-length if nonexistant.
		// ar: List of <ints> indicating existance and data type of  object. -1 = does not exist.
		// arg3: List of items containing data for each of the objects. GSI_LOG(false) if nonexistant.
		// The answer diiant came back as the wrong type
		// We only support safe arrays with 1 dimension

        DATAARRAY* pDataArray = 0; //&diAnswer

		// There are two DATAITEM safearrays in the answer; extract each one.
        DATAITEM * diGuidList;
        DATAITEM * diTypeList;
        DATAITEM * diValueList;

        DataArrayGetElement(pDataArray, 0, &diGuidList);
        DataArrayGetElement(pDataArray, 1, &diTypeList);
		DataArrayGetElement(pDataArray, 2, &diValueList);

        DATAARRAY * psaGuids = diGuidList->parray;
        DATAARRAY * psaTypes = diTypeList->parray;
        DATAARRAY * psaValues = diValueList->parray;
		unsigned long nElements = 0;
		DataArrayGetSize(psaGuids, &nElements);
        if (nElements != dwNumItems)
        {
			// We received a different # of items than we asked for!
            throw RET_UNEXPECTED;
        }

        DataArrayGetSize(psaTypes, &nElements);
        if (nElements != dwNumItems)
        {
            // We received a different # of items than we asked for!
            throw RET_UNEXPECTED;
        }

        DataArrayGetSize(psaValues, &nElements);
        if (nElements != dwNumItems)
        {
            // We received a different # of items than we asked for!
            throw RET_UNEXPECTED;
        }

        // Go through each element of the answer and determine the existance and
        // datatype of the corresponding requested data item.
         
        for (unsigned long i=0; i<dwNumItems; i++)
        {
			pAddResults[i].dwBlobSize = 0;
            pAddResults[i].pBlob = 0;
            pAddResults[i].dwAccessRights = READABLE | WRITEABLE;

            DATAITEM * diDataTypeResponse;
            DATAITEM * diDataGuidResponse;
            DATAITEM * diDataValueResponse;
            DataArrayGetElement(psaTypes, i, &diDataTypeResponse);
            if (diDataTypeResponse != 0)
            {
				throw hr;
            }
            if (diDataTypeResponse->dt != DT_I4)
            {
				throw RET_UNEXPECTED;
            }

			long long lDataType = diDataTypeResponse->llVal;

            DataArrayGetElement(psaGuids, i, &diDataGuidResponse);
            if (diDataGuidResponse!=0)
            {
				throw hr;
            }

            if (diDataGuidResponse->dt != DT_SSTR)
            {
				throw RET_UNEXPECTED;
            }
            
			SString bstrGuid = diDataGuidResponse->sstringVal;

            if (lDataType == -1)
            {
				// The requested itemID was not found in the  module.
				pErrors[i] = RET_UNKNOWNITMEID; // typo error in .H; should be UNKNOWNITEMID
				partialSuccessFlag = true;
            }
            else
            {
				// The itemID was found in ; obtain its native data type from the
				// Gensym data type returned.
				pErrors[i] = RET_OK;
				pAddResults[i].hServer = ++m_hNext;

				if (pErrors[i] == RET_OK)
				{
					// Try to coerce the native type (as returned by ) to the
					// requested type specified in the ITEMDEF structure.
                  
					/* FOR NOW, JUST LET ALL CONVERSION TYPES THROUGH.
                     IF THE DATA CAN'T BE CONVERTED, WE'LL DISCOVER THIS AT
                     SUBSCRIPTION-EVENT TIME. */
				}

				if (pErrors[i] == RET_OK)
				{
					// Create an Item for each data item.
					pItem = new Item;
					pErrors[i] = pItem->Init(m_hNext, &pItemArray[i], &pAddResults[i]);
	                if (pErrors[i] != RET_OK)
					{
						m_hNext--;
						delete pItem;
					}
					else
					{
						pItem->setGuid(bstrGuid);
						m_ItemMap.insert( ITEMMAP::value_type( m_hNext, pItem ) );
					}

					// Get the current data value and set it in the Item.
					DataArrayGetElement(psaValues, i, &diDataValueResponse);
					if (diDataValueResponse!=0)
					{
						throw hr;
					}
					pItem->setQuality( /*_QUALITY_GOOD = */ 0xC0 );
					pItem->SetValue(diDataValueResponse);
				} //if (lDataType == -1)
			} // for (i=0; i<dwNumItems; i++)
		}
	}
	catch (long e)
	{
		hr = e;
	}
   
	//LeaveCriticalSection(&m_CriticalSection);

	if (hr==RET_OK)
	{
		//hr = ReadData(); 
		if ( partialSuccessFlag == true )
		{
			hr = RET_FALSE;
		}
	}

	return hr;
}


long  CacheGroup::ValidateItems(unsigned long dwNumItems, ITEMDEF * pItemArray, bool bBlobUpdate, 
									ITEMRESULT ** ppAddResults, long ** ppErrors )
{
	ITEMRESULT * pAddResults = 0;
	long * pErrors = 0;
	long hr = RET_OK;
	//Item	*        pItem;
	bool partialSuccessFlag = false;
	unsigned long i;

	if (dwNumItems <= 0)
	{
		return RET_INVALIDARG;
	}

	*ppAddResults = 0;
	*ppErrors = 0;

	try
	{
		pAddResults = (ITEMRESULT*)malloc( sizeof( ITEMRESULT ) * dwNumItems );
		if ( pAddResults == 0 )
		{
			throw RET_OUTOFMEMORY;
		}      
	   
		pErrors = (long*)malloc( sizeof( long ) * dwNumItems);
		if ( pErrors == 0 )
		{
			throw RET_OUTOFMEMORY;
		}
		*ppAddResults = pAddResults;
		*ppErrors     = pErrors;
	}
	catch ( long e )
	{
		free(pAddResults);
		free( pErrors );      
		return e;
	}
	catch( ... )
	{
		free( pAddResults );
		free( pErrors );      
		return RET_UNEXPECTED;
	}   

	// Initialize memory
	memset(pAddResults, 0x00, sizeof(ITEMRESULT));
	for (i=0; i<dwNumItems; i++)
		pErrors[i] = RET_OK;


	try
	{
		// Build a query to  to see if the item ID's exist there
		// and if so, what their data types are. Returned data types
		// from  will have values as defined in gsi_main.h supplied by Gensym.
	}
	catch (long e)
	{
		hr = e;
	}
   
	if (hr == RET_OK && partialSuccessFlag == true)
		hr = RET_FALSE;

	return hr;
}

long CacheGroup::RemoveItems(unsigned long dwNumItems, unsigned long * phServer, long ** ppErrors )
{  
	long * pHr = 0;
	long hr = RET_OK;
	unsigned int i;

	try
	{
		pHr = (long *)malloc(sizeof( long ) * dwNumItems);
		*ppErrors = pHr;

	}
	catch ( ... )
	{
		free( pHr );
		return RET_FAIL;
	}   

	unsigned long numFoundItems = 0;
	DATAITEM * pItemGuids = new DATAITEM[dwNumItems];
	for (i=0; i<dwNumItems; i++)
	{
		DataItemInit(&pItemGuids[i]);
		pItemGuids[i].dt = DT_SSTR;
	}

	//EnterCriticalSection(&m_CriticalSection);

	for(  i= 0; i<dwNumItems; i++ )
	{
		ITEMMAP::iterator theIterator = m_ItemMap.find( phServer[i] );
		if( theIterator == m_ItemMap.end() )
		{
			pHr[i] = RET_INVALIDHANDLE;
			hr = RET_FALSE;
			continue;
		}

		// Item found, delete and remove from list
		Item * pItem = theIterator->second;      
		pItemGuids[numFoundItems++].sstringVal = SafeString(pItem->getGuid());
		m_ItemMap.erase( phServer[i] );
		delete pItem;
		pHr[i] = RET_OK;
	}

	// Send query to  to remove items from it's "group"
	DATAITEM * pdiCookie = new DATAITEM;
	DATAITEM * pdiGroupID = new DATAITEM;
	DATAITEM * pdiNumItems = new DATAITEM;
	DATAITEM * pdiItemID = new DATAITEM;

	DataItemInit(pdiCookie);
	DataItemInit(pdiGroupID);
	DataItemInit(pdiNumItems);
	DataItemInit(pdiItemID);

	pdiCookie->dt = DT_SSTR;
	pdiGroupID->dt = DT_SSTR;
	pdiNumItems->dt = DT_I4;
	pdiItemID->dt = DT_ARRAY | DT_DATAITEM;

	pdiCookie->sstringVal = SafeString("");
	pdiGroupID->sstringVal = SafeString(m_strGroupID.data());
	pdiNumItems->lVal = numFoundItems;
	DATAARRAY * psaItemID = DataArray(DT_DATAITEM, numFoundItems);

	if (psaItemID == NULL)
	{
		return RET_OUTOFMEMORY;
	}

	pdiItemID->parray = psaItemID;

	for (i=0; i<numFoundItems; i++)
	{
		DataArrayPutElement(psaItemID, i, &pItemGuids[i]);
	}

	// theQuestion
	// Object in  at which the question is directed
    // unique QueryID
    // Method to be invoked

	DataItemClear(pdiCookie);
	DataItemClear(pdiGroupID);
	DataItemClear(pdiNumItems);
	DataItemClear(pdiItemID);

   for (i=0; i<numFoundItems; i++)
   {
      DataItemClear(&pItemGuids[i]);
   }
   delete [] pItemGuids;
  
   //LeaveCriticalSection(&m_CriticalSection);

   return hr;
}

long CacheGroup::SetActiveState(unsigned long dwNumItems, unsigned long * phServer, bool bActive, long ** ppErrors )
{
	long * pHr = 0;
	long hr = RET_OK;

	try
	{
		pHr = new long[dwNumItems];
		*ppErrors = pHr;
	}
	catch ( ... )
	{
		delete [] pHr;
		return RET_FAIL;
	}

	for( unsigned int i= 0; i<dwNumItems; i++ )
	{
		ITEMMAP::iterator theIterator;

		theIterator = m_ItemMap.find( phServer[i] );

		if( theIterator == m_ItemMap.end() )
		{
			pHr[i] = RET_INVALIDHANDLE;
			hr = RET_FALSE;
			continue;
		}
		(*theIterator).second->SetActive( bActive );
		pHr[i] = RET_OK;
	}

	return hr;
}

long CacheGroup::SetClientHandles(unsigned long dwNumItems, unsigned long * phServer, 
									unsigned long * phClient, long ** ppErrors)
{
	long *   pHr = 0;
	long     hr = RET_OK;

	try
	{
		pHr = new long[dwNumItems];
		*ppErrors = pHr;
	}
	catch ( ... )
	{
		delete [] pHr;
		return RET_FAIL;
	}

	for( unsigned int i= 0; i<dwNumItems; i++ )
	{
		ITEMMAP::iterator theIterator = m_ItemMap.find( phServer[i] );
		if( theIterator == m_ItemMap.end() )
		{
			pHr[i] = RET_INVALIDHANDLE;
			hr = RET_FALSE;
			continue;
		}
		(*theIterator).second->SetClientHandle( phClient[i] );
		pHr[i] = RET_OK;
	}
	return hr;
}

long CacheGroup::SetDatatypes(unsigned long dwNumItems, unsigned long * phServer, 
							DATATYPE * pRequestedDatatypes, long ** ppErrors )
{
	long *   pHr = 0;
	long     hr = RET_OK;

	try
	{
		pHr = new long[dwNumItems];
		*ppErrors = pHr;
	}
	catch ( ... )
	{
		delete [] pHr;
		return RET_FAIL;
	}

	for( unsigned int i= 0; i<dwNumItems; i++ )
	{
		ITEMMAP::iterator theIterator;
		theIterator = m_ItemMap.find( phServer[i] );

		if( theIterator == m_ItemMap.end() )
		{
			pHr[i] = RET_INVALIDHANDLE;
			hr = RET_FALSE;
			continue;
		}

		pHr[i] = (*theIterator).second->SetDataType( pRequestedDatatypes[i] );

		if(pHr[i]==RET_FAIL)
		{
			hr = RET_FALSE;
		}
	}
	return hr;
}

long CacheGroup::CreateEnumerator(void ** ppCacheGroup)
{
	return RET_NOTIMPL;
}
