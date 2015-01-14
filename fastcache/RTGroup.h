#pragma once
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


class IRTGroup 
{
public:
	virtual long Init() = 0;
};

class IRTGroupStateMgt 
{
public:
	virtual long CloneGroup(wchar_t * szName, long riid, void * ppUnk) = 0;
	virtual long SetName(wchar_t * szName) = 0;
	virtual long SetState(unsigned long * pRequestedUpdateRate,  unsigned long * pRevisedUpdateRate, bool * pActive, long * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,long * phClientGroup) = 0;
	virtual long GetState(unsigned long * pUpdateRate, bool * pActive, wchar_t * * ppName, long  * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,long * phClientGroup,long * phServerGroup) = 0;
};

class IRTItemMgt
{
public:
	virtual long CreateEnumerator(unsigned int riid, void * ppUnk) = 0;
	virtual long SetDatatypes(unsigned long dwNumItems, long * phServer, DATATYPE * pRequestedDatatypes, long ** ppErrors) = 0;
	virtual long SetClientHandles(unsigned long dwNumItems,long  * phServer,long  * phClient, long   ** ppErrors) = 0;
	virtual long SetActiveState(unsigned long dwNumItems,   long  * phServer, bool bActive,  long   ** ppErrors) = 0;
	virtual long RemoveItems(unsigned long dwNumItems, long  * phServer, long ** ppErrors) = 0;
	virtual long ValidateItems(unsigned long dwNumItems, ITEMDEF * pItemArray, bool bBlobUpdate, ITEMRESULT ** ppValidationResults, long ** ppErrors) = 0;
	virtual long AddItems(unsigned long dwNumItems, ITEMDEF * pItemArray, ITEMRESULT ** ppAddResults, long ** ppErrors) = 0;
};

/*
class IRTDataObject
{
public:
	virtual long GetData(STGMEDIUM  *pmedium) = 0;
	virtual long GetDataHere(FORMATETC  *pformatetc, STGMEDIUM  *pmedium) = 0;
	virtual long QueryGetData(FORMATETC  *pformatetc) = 0;
	virtual long GetCanonicalFormatEtc(FORMATETC  *pformatectIn, FORMATETC  *pformatetcOut) = 0;
	virtual long SetData(FORMATETC  *pformatetc, STGMEDIUM  *pmedium, bool fRelease) = 0;
	virtual long EnumFormatEtc(unsigned long dwDirection, IEnumFORMATETC  * *ppenumFormatEtc) = 0;
	virtual long DAdvise(FORMATETC *pfe,unsigned long advf,IAdviseSink *pAdvSink,unsigned long *pdwConnection) = 0;
	virtual long DUnadvise(unsigned long dwConnection) = 0;
	virtual long EnumDAdvise(IEnumSTATDATA  * *ppenumAdvise) = 0;
};
*/

class IRTAsyncIO
{
public:
	virtual long Cancel( unsigned long dwTransactionID) = 0;
	virtual long Refresh( unsigned long dwConnection, DATASOURCE dwSource, unsigned long * pTransactionID) = 0;
	virtual long Write( unsigned long dwConnection, unsigned long dwNumItems,  long * phServer, Item * pItemValues,  unsigned long * pTransactionID, long ** ppErrors) = 0;
	virtual long Read(unsigned long dwConnection, DATASOURCE dwSource,unsigned long dwNumItems, long * phServer,unsigned long * pTransactionID, long ** ppErrors) = 0;
};

class IRTSyncIO
{
public:
	virtual long Write(unsigned long dwNumItems, long * phServer, Item * pItemValues, long ** ppErrors) = 0;
	virtual long Read(DATASOURCE dwSource, unsigned long dwNumItems,  long * phServer, ITEMSTATE ** ppItemValues, long ** ppErrors) = 0;\
};





class RTGroup : 
	public IRTGroup, 
	public IRTGroupStateMgt, 
	public IRTItemMgt, 
	public IRTAsyncIO, 
	public IRTSyncIO 
{
public:
	RTGroup();
	~RTGroup();

		void setName( const std::wstring & rName )
	{
		m_strName = rName;
	}

	const std::wstring & getName() const
	{
		return m_strName;
	}

	void setGroupID( const std::wstring & rGroupID )
	{
		m_strGroupID = rGroupID;
	}

	const std::wstring & getGroupID() const
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
   
	long getServerGroup()
	{ 
		return m_hServerGroup; 
	}
   
	long getClientGroup()
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

	void setServerGroup(long hServerGroup)
	{ 
		m_hServerGroup = hServerGroup; 
	}

	void setClientGroup(long hClientGroup )
	{ 
		m_hClientGroup = hClientGroup;
	}

	long setGroupItemData(DATAITEM* pvarGuids, DATAITEM* pvarValues, DATAITEM* pvarQualities);

	unsigned long itemCount();
	long ReadData();

	long advanceTime(unsigned long deltaTime, unsigned long * pCount, Item **& ppItems);
	long notifySinks(unsigned long deltaTime);

//	IRTGroup
	long Init();

//	IRTGroupStateMgt 
	long CloneGroup(wchar_t * szName, long riid, void * ppUnk);
	long SetName(wchar_t * szName);
	long SetState(unsigned long * pRequestedUpdateRate,  unsigned long * pRevisedUpdateRate, bool * pActive, long * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,long * phClientGroup);
	long GetState(unsigned long * pUpdateRate, bool * pActive, wchar_t * * ppName, long  * pTimeBias,float * pPercentDeadband,unsigned long * pLCID,long * phClientGroup,long * phServerGroup);

//	IRTItemMgt
	long CreateEnumerator(unsigned int riid, void * ppUnk);
	long SetDatatypes(unsigned long dwNumItems, long * phServer, DATATYPE * pRequestedDatatypes, long ** ppErrors);
	long SetClientHandles(unsigned long dwNumItems,long  * phServer,long  * phClient, long   ** ppErrors);
	long SetActiveState(unsigned long dwNumItems,   long  * phServer, bool bActive,  long   ** ppErrors);
	long RemoveItems(unsigned long dwNumItems, long  * phServer, long ** ppErrors);
	long ValidateItems(unsigned long dwNumItems, ITEMDEF * pItemArray, bool bBlobUpdate, ITEMRESULT ** ppValidationResults, long ** ppErrors);
	long AddItems(unsigned long dwNumItems, ITEMDEF * pItemArray, ITEMRESULT ** ppAddResults, long ** ppErrors);

//	IRTAsyncIO
	long Cancel( unsigned long dwTransactionID);
	long Refresh( unsigned long dwConnection, DATASOURCE dwSource, unsigned long * pTransactionID);
	long Write( unsigned long dwConnection, unsigned long dwNumItems,  long * phServer, Item * pItemValues,  unsigned long * pTransactionID, long ** ppErrors);
	long Read(unsigned long dwConnection, DATASOURCE dwSource,unsigned long dwNumItems, long * phServer,unsigned long * pTransactionID, long ** ppErrors);

//	IRTSyncIO
	long Write(unsigned long dwNumItems, long * phServer, Item * pItemValues, long ** ppErrors);
	long Read(DATASOURCE dwSource, unsigned long dwNumItems,  long * phServer, ITEMSTATE ** ppItemValues, long ** ppErrors);\

private:
	ADVISESINKVECTOR  m_adviseSinkVector;
	std::wstring m_strName;
	std::wstring m_strGroupID;
	int m_nFreeExtra;
	double m_Deadband;
	unsigned long m_LCID;
	long m_TimeBias;
	bool m_bActive;
	wchar_t * m_szName;
	long m_hServerGroup;
	long m_hClientGroup;
	unsigned long m_ulRevisedRate;
	unsigned long m_ulAccumTime;
	ITEMMAP m_ItemMap;
	long m_hNext;

	unsigned long m_dwDataCallback;
	unsigned long m_dwDatatimeCallback;
	unsigned long m_dwWriteCallback;

	unsigned int m_data;
	unsigned int m_datatime;
	unsigned int m_write;
	//Thread * m_pThread;

   static unsigned long g_ulTransactionID;

   friend class CSendStream;
};

