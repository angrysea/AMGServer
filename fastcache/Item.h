#pragma once
#include <string>

#include "rtds.h"
#include "types.h"
#include "DataItem.h"
#include "SafeString.h"

class Item
{
public:
	Item();
	Item(const Item &) = delete;

	Item(Item && aCopy) :
		m_hServerItem{ std::move(aCopy.m_hServerItem) },
		m_hClientItem{ std::move(aCopy.m_hClientItem) },
		m_szItemID{ std::move(aCopy.m_szItemID) },
		m_szAccessPath{ std::move(aCopy.m_szAccessPath) },
		m_bActive{ std::move(aCopy.m_bActive) },
		m_Quality{ std::move(aCopy.m_Quality) },
		m_bDirty{ std::move(aCopy.m_bDirty) },
		m_LastWriteError{ std::move(aCopy.m_LastWriteError) },
		m_AsyncMask{ std::move(aCopy.m_AsyncMask) },
		m_vtCanonicalDataType{ std::move(aCopy.m_vtCanonicalDataType) },
		m_vtRequested{ std::move(aCopy.m_vtRequested) },
		m_bstrGuid{ std::move(aCopy.m_bstrGuid) },
		m_BeenUpdated{ std::move(aCopy.m_BeenUpdated) },
		m_TimeStamp{ std::move(aCopy.m_TimeStamp) }

	{
		DataItemMove(&m_diValue, &(aCopy.m_diValue));
	}

	Item & operator=(Item & aCopy) = delete;

	Item & operator=(Item && aCopy)
	{
		m_hServerItem = std::move(aCopy.m_hServerItem);
		m_hClientItem = std::move(aCopy.m_hClientItem);
		m_szItemID = std::move(aCopy.m_szItemID);
		m_szAccessPath = std::move(aCopy.m_szAccessPath);
		m_bActive = std::move(aCopy.m_bActive);
		m_Quality = std::move(aCopy.m_Quality);
		m_bDirty = std::move(aCopy.m_bDirty);
		m_LastWriteError = std::move(aCopy.m_LastWriteError);
		m_AsyncMask = std::move(aCopy.m_AsyncMask);
		m_vtCanonicalDataType = std::move(aCopy.m_vtCanonicalDataType);
		m_vtRequested = std::move(aCopy.m_vtRequested);
		m_bstrGuid = std::move(aCopy.m_bstrGuid);
		m_BeenUpdated = std::move(aCopy.m_BeenUpdated);
		m_TimeStamp = std::move(aCopy.m_TimeStamp);

		DataItemMove(&m_diValue, &(aCopy.m_diValue));
	}

	long Init(unsigned long serverItem, ITEMDEF * itemDef, ITEMRESULT * itemResult);

	virtual ~Item()
	{
		DataItemClear(&m_diValue);
		if(m_bstrGuid)
		{
			SafeStringFree(m_bstrGuid);
		}
	}

	long Init(DATATYPE dt);

	long GetAttributeItem(ITEMATTRIBUTES *ITEM);
	long IsDirty(bool * bDirtyFlag);

	unsigned short getQuality() { return m_Quality; }
	void setQuality( unsigned short wQuality ) { m_BeenUpdated = true; m_Quality = wQuality; }

	long SetValue(const DATAITEM * di);
	long GetValue(DATASOURCE ds, DATAITEM * di, unsigned short *qual, time_t * time);
	long SetDataType(DATATYPE v);

	long GetActive(bool *bActive)
	{
		*bActive = m_bActive;
		return RET_OK;
	}

	long setGuid(SString bstrGuid)
	{
		m_bstrGuid = bstrGuid;
		return RET_OK;
	}

	SString getGuid()
	{
		return m_bstrGuid;
	}

	long SetActive(bool active)
	{
		m_bActive = active;
		if ( m_bActive != 0 ) 
		{
			SetDirty( true );
		}
		else 
		{
			SetDirty( false );
		}
		return RET_OK;
	}

	long GetDataType(DATATYPE * v)
	{
		*v = m_vtCanonicalDataType;
		return RET_OK;
	}

	long SetDirty(bool bDirtyFlag)
	{
		m_bDirty = bDirtyFlag;
		return RET_OK;
	}

	long GetServerHandle(unsigned long * hServer)
	{
		*hServer = m_hServerItem;
		return 0;
	}

	long SetServerHandle(unsigned long hServer)
	{
		m_hServerItem = hServer;
		return 0;
	}

	long GetClientHandle(unsigned long * m_hClient)
	{
		*m_hClient = m_hClientItem;
		return 0;
	}

	long SetClientHandle(unsigned long m_hClient )
	{
		m_hClientItem = m_hClient;
		return 0;
	}

	std::string getItemID()
	{
		return m_szItemID;
	}

	static bool isEqual(const DATAITEM * di1, const DATAITEM * di2);
   
private:
	long m_hServerItem;
	long m_hClientItem;
	std::string m_szItemID;
	std::string m_szAccessPath;
	bool m_bActive;
	unsigned short m_Quality;
	bool m_bDirty;
	long m_LastWriteError;
	unsigned short m_AsyncMask;
	DATATYPE m_vtCanonicalDataType;
	DATATYPE m_vtRequested;
	DATAITEM m_diValue;
	SString m_bstrGuid;
	bool m_BeenUpdated;
	time_t m_TimeStamp;
};
