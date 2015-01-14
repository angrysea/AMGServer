#include <stdio.h>
#include <stdlib.h>
 #include <sstream>
 #include <string>

#include "CacheServer.h"
#include "CacheGroup.h"
#include "Item.h"
#include "DataItem.h"
#include "DataArray.h"
#include "JNIException.h"

static char	lpszError[] = "Server has no error support";


CacheServer * CacheServer::theCacheServer = 0;
int JNIException::lastError = 0;

CacheServer::CacheServer() :
			m_hNextGroup( 0 ),
			m_strError( lpszError ),
			m_lTransID( 0 )
{
}

CacheServer::~CacheServer()
{
	theCacheServer = 0;
}

CacheServer * CacheServer::getCacheServer()
{
	if(theCacheServer==0) 
	{
		theCacheServer = new CacheServer();
	}
	return theCacheServer;
}

bool CacheServer::putValue(const char * key, const DATAITEM * value)
{
	Item * item = new Item;
	item->Init(value->dt);
	item->SetValue(value);
	root.insert(key, new InstanceNode(item));

	return true;
}

bool CacheServer::getValue(const char * key, DATAITEM * dataItem) 
{
	unsigned short qual;
	time_t * time = 0;
	DataItemInit(dataItem);
	InstanceNode * node = root.getNode(key);
	Item * item = node->getValue();
	item->GetValue(DS_CACHE, dataItem, &qual, time);

	return true;
}

bool CacheServer::getChildren(const char * key, DATAITEM * dataItem) 
{
	unsigned short qual;
	time_t * time = 0;
	DataItemInit(dataItem);
	std::vector<InstanceNode * > theList;
	root.getNodes(key, theList);
	int count = theList.size();
	DATAARRAY * da = DataArray(DT_DATAITEM, count);
	DATAITEM * di = new DATAITEM;
	long i = 0;
	for(InstanceNode * node : theList) 
	{
		Item * item = node->getValue();
		item->GetValue(DS_CACHE, di, &qual, time);
		DataArrayPutElement(da, i, di);
		i++;
	}
	dataItem->dt = DT_ARRAY | DT_DATAITEM;
	dataItem->parray = da;

	return true;
}


long CacheServer::CreateGroupEnumerator(ENUMSCOPE dwScope, CacheGroup ** ppGroup)
{
	return RET_NOTIMPL;
}

long CacheServer::RemoveGroup(unsigned long hServerGroup, bool bForce)
{
	long hr = RET_OK;
	//"Entering COPCServer::RemoveGroup\n"
	//EnterCriticalSection(&m_CriticalSection);

	try
	{
		GROUPMAP::iterator theIterator;
		theIterator = m_GroupMap.find(hServerGroup);

		if( theIterator != m_GroupMap.end() )
		{         
			CacheGroup * pGroup = theIterator->second;         
			pGroup->setActive(false);
			//"Removing Group %d\n"), pGroup->getServerGroup(); 
			m_GroupMap.erase( theIterator );
		}
	}
	catch( ... )
	{
		//"Unknown error caught in COPCServer::RemoveGroup\n"
		hr = RET_UNEXPECTED;
	}

	//LeaveCriticalSection(&m_CriticalSection);
	//ATLTRACE( _T("Leaving COPCServer::RemoveGroup\n") );
     
	return hr;
}

long CacheServer::GetStatus(SERVERSTATUS ** ppServerStatus)
{
	unsigned long hr = RET_OK;

	if(ppServerStatus != 0)
	{
		SERVERSTATUS * pServerStatus = (SERVERSTATUS *)malloc(sizeof(SERVERSTATUS));
		pServerStatus->dwServerState = STATUS_RUNNING;
		pServerStatus->dwGroupCount = 0;
		pServerStatus->dwBandWidth = 0;
		pServerStatus->wMajorVersion = 0;
		pServerStatus->wMinorVersion = 0;
		pServerStatus->wBuildNumber = 0;
		pServerStatus->wReserved = 44;
		*ppServerStatus = pServerStatus;
	}
	else
	{
		hr = RET_INVALIDARG;
	}

	return hr;
}

long CacheServer::GetGroupByName(const char *szName, CacheGroup ** ppGroup)
{
	long hr = RET_FAIL;
   
	if(ppGroup==0)
	{
		return RET_INVALIDARG;
	}

	*ppGroup = 0;
	GROUPMAP::iterator theIterator;
	string groupName(szName);
	theIterator = m_GroupMap.begin();

	while(theIterator != m_GroupMap.end())
	{
		if((*theIterator).second->getName() == groupName) 
		{
			break;
		}
		advance( theIterator, 1 );
	}
   
	return hr; 
}

long CacheServer::GetErrorString(long dwError, char ** ppString)
{
	return RET_NOTIMPL;
}

long CacheServer::AddGroup(const char * szName, bool bActive, double dRequestedUpdateRate,
	unsigned long hClientGroup, long * pTimeBias, float * pPercentDeadband, unsigned long * phServerGroup, 
	unsigned long * pRevisedUpdateRate, CacheGroup ** ppGroup)
{
	unsigned long hr = RET_FALSE;
	CacheGroup * pGroup = 0;
	string strName;

	try
	{
		//EnterCriticalSection(&m_CriticalSection);
		if( strlen( szName ) == 0 )
		{
			basic_ostringstream<char> ostr;
			ostr << m_hNextGroup;
			strName = ostr.str();
		}
		else
		{
			strName = szName;
		}
	
		//Check to see if a group by this name already exists
		GetGroupByName(strName.data(), &pGroup);

		if(pGroup!=0)
		{
			throw RET_E_DUPLICATE_NAME;
		}

		// note DISPPARAM arguments are reversed
		DATAITEM * pvarCookie = new DATAITEM;
		DATAITEM * pvarGroupID = new DATAITEM;
		DATAITEM * pvarUpdateRate = new DATAITEM;
		DATAITEM * pvarActiveStatus = new DATAITEM;
		DATAITEM * pvarPercentDeadband = new DATAITEM;

		DataItemInit(pvarCookie);
		DataItemInit(pvarGroupID);
		DataItemInit(pvarUpdateRate);
		DataItemInit(pvarActiveStatus);
		DataItemInit(pvarPercentDeadband);

		pvarCookie->dt = DT_SSTR;
		pvarGroupID->dt = DT_SSTR;
		pvarUpdateRate->dt = DT_R8;
		pvarActiveStatus->dt = DT_I4;
		pvarPercentDeadband->dt = DT_R8;

		pvarCookie->sstringVal = SafeString("");
		//pvarGroupID->sstringVal = SafeString(strGuidGroup.data());
		pvarUpdateRate->dblVal = (double)(dRequestedUpdateRate / 1000.);
		pvarActiveStatus->lVal = bActive == true ? 1 : 0;
		pvarPercentDeadband->dblVal = 0.0;

		DataItemClear(pvarCookie);
		DataItemClear(pvarGroupID);
		DataItemClear(pvarUpdateRate);
		DataItemClear(pvarActiveStatus);
		DataItemClear(pvarPercentDeadband);

		// Now get answer back from  query. Answer will either be a VT_ERROR
		// or a VT_R8 containing the revised rate of the group.
		DATAITEM * varAnswer = new DATAITEM;
		DataItemInit(varAnswer);



		if (varAnswer->dt == DT_ERROR) // an error has occurred
		{
			hr = varAnswer->scode;
			DataItemClear(varAnswer);
			delete varAnswer;
			throw hr;
		}

		if(varAnswer->dt != DT_R8)
		{
			DataItemClear(varAnswer);
			delete varAnswer;
			throw RET_INVALIDARG;
		}

		// Get the RevisedRate from the answer. Format is a double in seconds.
		double dRevisedRate = varAnswer->dblVal;

		DataItemClear(varAnswer);
		delete varAnswer;

		pGroup = new CacheGroup;

		if ((hr=pGroup->Init()) != RET_OK)
		{
			throw hr;
		}

		pGroup->setServerGroup( m_hNextGroup );
		pGroup->setClientGroup( hClientGroup );
		pGroup->setRevisedRate((dRevisedRate * 1000.));
		pGroup->setActive( bActive );
		pGroup->setName( strName );
		//pGroup->setGroupID(strGuidGroup);
		if (pPercentDeadband == 0)
			pGroup->setDeadband(0.);
		else
			pGroup->setDeadband(*pPercentDeadband);
		//pGroup->setLCID( dwLCID );

		if( pTimeBias != 0 ) 
		{
			pGroup->setTimeBias( *pTimeBias );
		}
		else
		{
			pGroup->setTimeBias( 0 );
		}

		// Returned server handle
		*phServerGroup = m_hNextGroup;
		// returned revise rate
		*pRevisedUpdateRate = pGroup->getRevisedRate();

		m_GroupMap.insert( GROUPMAP::value_type( m_hNextGroup, pGroup ) );

		m_hNextGroup++;

	}
	catch (long e)
	{
		hr = e;
	}
	catch ( ... )
	{
		hr = RET_UNEXPECTED;
	}

	//LeaveCriticalSection(&m_CriticalSection);
     
	return hr;
}


long CacheServer::advanceTime(unsigned long deltaTime)
{
	GROUPMAP::iterator theIterator;
	unsigned long itemCount = 0;

   //EnterCriticalSection(&m_CriticalSection);

   theIterator = m_GroupMap.begin();
   while (theIterator != m_GroupMap.end())
   {
      itemCount += (*theIterator).second->notifySinks(deltaTime);
      advance(theIterator, 1);
   }

   //LeaveCriticalSection(&m_CriticalSection);

   return RET_OK;
}

