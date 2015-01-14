#pragma once
// Debug 
// -classpath C:\projects\adaptinet\sdk\target\classes;C:\projects\adaptinet\lib\jtds-1.2.4.jar;C:\Users\grafant\AppData\.m2\repository\log4j\log4j\1.2.16\log4j-1.2.16.jar;C:\projects\adaptinet\fastcachej\target;C:\projects\adaptinet\InstrumentCache\bin -Djava.library.path=C:\projects\adaptinet\fastcachej\fastcache\Debug org.adaptinet.transceiver.Transceiver -config C:/projects/adaptinet/InstrumentCache/conf/transceiver1.properties
//-classpath C:\projects\adaptinet\sdk\target\classes;C:\projects\adaptinet\lib\jtds-1.2.4.jar;C:\Users\grafant\AppData\.m2\repository\log4j\log4j\1.2.16\log4j-1.2.16.jar;C:\projects\adaptinet\fastcachej\target;C:\projects\adaptinet\InstrumentCache\bin -Djava.library.path=C:\projects\adaptinet\fastcachej\fastcache\Debug com.db.instrument.cache.InstrumentCache
#include <map>
#include <string>
#include <vector>
using namespace std;

#include "rtds.h"
#include "DataItem.h"

class Item;
class AdviseSink;

typedef std::map< unsigned long, Item *, std::less<unsigned long> > ITEMMAP;
typedef std::vector< AdviseSink * > ADVISESINKVECTOR;

class ICacheGroup 
{
public:
	virtual long Init() = 0;
};

class ICacheGroupStateMgt 
{
public:
	virtual long CloneGroup(char * szName, void ** ppCacheGroupClone) = 0;
	virtual long SetName(char * szName) = 0;
	virtual long SetState(unsigned long * pRequestedUpdateRate,  unsigned long * pRevisedUpdateRate, bool * pActive, long * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,unsigned long * phClientGroup) = 0;
	virtual long GetState(unsigned long * pUpdateRate, bool * pActive, char * * ppName, long  * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,unsigned long * phClientGroup,unsigned long * phServerGroup) = 0;
};

class ICacheItemMgt
{
public:
	virtual long CreateEnumerator(void ** ppUnk) = 0;
	virtual long SetDatatypes(unsigned long dwNumItems, unsigned long * phServer, DATATYPE * pRequestedDatatypes, long ** ppErrors) = 0;
	virtual long SetClientHandles(unsigned long dwNumItems, unsigned long * phServer,unsigned long  * phClient, long ** ppErrors) = 0;
	virtual long SetActiveState(unsigned long dwNumItems, unsigned long * phServer, bool bActive, long ** ppErrors) = 0;
	virtual long RemoveItems(unsigned long dwNumItems, unsigned long * phServer, long ** ppErrors) = 0;
	virtual long ValidateItems(unsigned long dwNumItems, ITEMDEF * pItemArray, bool bBlobUpdate, ITEMRESULT ** ppValidationResults, long ** ppErrors) = 0;
	virtual long AddItems(unsigned long dwNumItems, ITEMDEF * pItemArray, ITEMRESULT ** ppAddResults, long ** ppErrors) = 0;
};

class ICacheAsyncIO
{
public:
	virtual long Cancel( unsigned long dwTransactionID) = 0;
	virtual long Refresh( unsigned long dwConnection, DATASOURCE dwSource, unsigned long * pTransactionID) = 0;
	virtual long Write( unsigned long dwConnection, unsigned long dwNumItems,  unsigned long * phServer, Item * pItemValues,  unsigned long * pTransactionID, long ** ppErrors) = 0;
	virtual long Read(unsigned long dwConnection, DATASOURCE dwSource,unsigned long dwNumItems, unsigned long * phServer,unsigned long * pTransactionID, long ** ppErrors) = 0;
};

class ICacheSyncIO
{
public:
	virtual long Write(unsigned long dwNumItems, unsigned long * phServer, Item * pItemValues, long ** ppErrors) = 0;
	virtual long Read(DATASOURCE dwSource, unsigned long dwNumItems,  unsigned long * phServer, ITEMSTATE ** ppItemValues, long ** ppErrors) = 0;
};


class CacheGroup : 
	public ICacheGroup, 
	public ICacheGroupStateMgt, 
	public ICacheItemMgt, 
	public ICacheAsyncIO, 
	public ICacheSyncIO 
{
public:
	CacheGroup();
	~CacheGroup();

	void setName( const std::string & rName )
	{
		m_strName = rName;
	}

	const std::string & getName() const
	{
		return m_strName;
	}

	void setGroupID( const std::string & rGroupID )
	{
		m_strGroupID = rGroupID;
	}

	const std::string & getGroupID() const
	{
		return m_strGroupID;
	}

	Item * getItem(unsigned long hItem)
	{
		ITEMMAP::iterator theIterator;
		theIterator = m_ItemMap.find( hItem );
		return (theIterator == m_ItemMap.end()) ? 0 : (*theIterator).second;	
	}

	unsigned long getRevisedRate()
	{ 
		return m_ulRevisedRate; 
	}
   
	double getDeadband()
	{ 
		return m_Deadband; 
	}
   
	unsigned long getLCID()
	{ 
		return m_LCID; 
	}
   
	long getTimeBias()
	{ 
		return m_TimeBias; 
	}
   
	bool getActive()
	{ 
		return m_bActive; 
	}
   
	unsigned long getServerGroup()
	{ 
		return m_hServerGroup; 
	}
   
	unsigned long getClientGroup()
	{ 
		return m_hClientGroup; 
	}

	void setRevisedRate(unsigned long dwRevisedRate)
	{ 
		m_ulRevisedRate = dwRevisedRate; 
		m_ulAccumTime = dwRevisedRate+1;
	}

	void setDeadband(float fDeadband)
	{ 
		m_Deadband = fDeadband; 
	}

	void setLCID(unsigned long ulLCID)
	{ 
		m_LCID = ulLCID; 
	}

	void setTimeBias(long lTimeBias)
	{ 
		m_TimeBias = lTimeBias; 
	}

	void setActive(bool bActive)
	{ 
		m_bActive = bActive; 
	}

	void setServerGroup(unsigned long hServerGroup)
	{ 
		m_hServerGroup = hServerGroup; 
	}

	void setClientGroup(unsigned long hClientGroup )
	{ 
		m_hClientGroup = hClientGroup;
	}

	long setGroupItemData(DATAITEM* pvarGuids, DATAITEM* pvarValues, DATAITEM* pvarQualities);

	unsigned long itemCount();
	long ReadData();

	long advanceTime(unsigned long deltaTime, unsigned long * pCount, Item **& ppItems);
	long notifySinks(unsigned long deltaTime);

//	ICacheGroup
	long Init();

//	ICacheGroupStateMgt 
	long CloneGroup(char * szName, void ** ppCacheGroupClone);
	long SetName(char * szName);
	long SetState(unsigned long * pRequestedUpdateRate,  unsigned long * pRevisedUpdateRate, bool * pActive, long * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,unsigned long * phClientGroup);
	long GetState(unsigned long * pUpdateRate, bool * pActive, char * * ppName, long  * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,unsigned long * phClientGroup,unsigned long * phServerGroup);

//	ICacheItemMgt
	long CreateEnumerator(void ** ppCacheGroup);
	long SetDatatypes(unsigned long dwNumItems, unsigned long * phServer, DATATYPE * pRequestedDatatypes, long ** ppErrors);
	long SetClientHandles(unsigned long dwNumItems,unsigned long  * phServer,unsigned long  * phClient, long   ** ppErrors);
	long SetActiveState(unsigned long dwNumItems,   unsigned long  * phServer, bool bActive,  long   ** ppErrors);
	long RemoveItems(unsigned long dwNumItems, unsigned long  * phServer, long ** ppErrors);
	long ValidateItems(unsigned long dwNumItems, ITEMDEF * pItemArray, bool bBlobUpdate, ITEMRESULT ** ppValidationResults, long ** ppErrors);
	long AddItems(unsigned long dwNumItems, ITEMDEF * pItemArray, ITEMRESULT ** ppAddResults, long ** ppErrors);

//	ICacheAsyncIO
	long Cancel(unsigned long dwTransactionID);
	long Refresh(unsigned long dwConnection, DATASOURCE dwSource, unsigned long * pTransactionID);
	long Write(unsigned long dwConnection, unsigned long dwNumItems,  unsigned long * phServer, Item * pItemValues,  unsigned long * pTransactionID, long ** ppErrors);
	long Read(unsigned long dwConnection, DATASOURCE dwSource,unsigned long dwNumItems, unsigned long * phServer,unsigned long * pTransactionID, long ** ppErrors);

//	ICacheSyncIO
	long Write(unsigned long dwNumItems, unsigned long * phServer, Item * pItemValues, long ** ppErrors);
	long Read(DATASOURCE dwSource, unsigned long dwNumItems,  unsigned long * phServer, ITEMSTATE ** ppItemValues, long ** ppErrors);\

private:
	ADVISESINKVECTOR  m_adviseSinkVector;
	std::string m_strName;
	std::string m_strGroupID;
	int m_nFreeExtra;
	double m_Deadband;
	unsigned long m_LCID;
	long m_TimeBias;
	bool m_bActive;
	char * m_szName;
	unsigned long m_hServerGroup;
	unsigned long m_hClientGroup;
	unsigned long m_ulRevisedRate;
	unsigned long m_ulAccumTime;
	ITEMMAP m_ItemMap;
	unsigned long m_hNext;

	unsigned long m_dwDataCallback;
	unsigned long m_dwDatatimeCallback;
	unsigned long m_dwWriteCallback;

	unsigned int m_data;
	unsigned int m_datatime;
	unsigned int m_write;
	//Thread * m_pThread;

	static unsigned long g_ulTransactionID;

	friend class CacheSendStream;
};

