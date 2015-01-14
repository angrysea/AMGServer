#include <string.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include "Item.h"

#define _GOOD_QUALITY 0xc0
#define _BAD_QUALITY 0x00
#define _UNCERTAIN_QUALITY 0x40


Item::Item() :
	m_hServerItem(0),
	m_hClientItem(0),
	m_bActive(false),
	m_Quality(_BAD_QUALITY),
	m_bDirty(true),
	m_LastWriteError( RET_OK ),
	m_AsyncMask( 0 ),
	m_vtCanonicalDataType(DT_EMPTY),
	m_vtRequested(DT_EMPTY),
	m_bstrGuid(0),
	m_BeenUpdated(false)
{
	DataItemInit( &m_diValue );
}

long Item::Init(unsigned long serverItem, ITEMDEF * itemDef, ITEMRESULT * itemResult)
{
	long hr = RET_OK;

	// Initialize the itemResult blob
	itemResult->pBlob = 0;
	itemResult->dwBlobSize = 0;

	try
	{
		DataItemInit( &m_diValue );
      
		m_vtCanonicalDataType = itemResult->vtCanonicalDataType;
		m_vtRequested = itemDef->vtRequestedDataType;
		m_Quality = _GOOD_QUALITY;

		// &m_TimeStamp;
	   
		if ( itemDef->szItemID != 0 )
		{
			m_szItemID = _strdup(itemDef->szItemID);
		}
      
		if ( itemDef->szAccessPath != 0 )
		{
			m_szAccessPath = _strdup(itemDef->szAccessPath);
		}

		m_hServerItem = serverItem;
		m_hClientItem = itemDef->hClient;
		m_bActive = itemDef->bActive !=0 ? true : false;

		//SetDirty( true );

		// Redundant but OK
		itemResult->hServer = m_hServerItem;
		itemResult->vtCanonicalDataType = m_vtCanonicalDataType;
		itemResult->dwAccessRights = WRITEABLE | READABLE;
		itemResult->pBlob = 0;
		itemResult->dwBlobSize = 0;
   
	}
	catch (long e)
	{
		hr = e;
	}
	catch ( ... )
	{
		hr = RET_UNEXPECTED;
	}

	return hr;
}

long Item::Init(DATATYPE dt)
{
	long hr = RET_OK;

	try
	{
		DataItemInit( &m_diValue );
		m_vtCanonicalDataType = dt;
		m_Quality = _GOOD_QUALITY;
		time(&m_TimeStamp);
		m_bActive = true;
	}
	catch ( long e )
	{
		hr = e;
	}
	catch ( ... )
	{
		hr = RET_UNEXPECTED;
	}

	return hr;
}

long Item::GetAttributeItem(ITEMATTRIBUTES * item)
{
	item->bActive = m_bActive;
	item->hServer = m_hServerItem;
	item->hClient = m_hClientItem;
	item->dwAccessRights = READABLE | WRITEABLE;
	item->dwBlobSize = 0;
	item->pBlob = 0;
	item->vtRequestedDataType = m_vtRequested;
	item->vtCanonicalDataType = m_vtCanonicalDataType;
	item->dwEUType = NOENUM;
	item->vEUInfo->dt = DT_EMPTY;
  
	//item->szAccessPath = new char[m_szAccessPath.size()];
	//m_szAccessPath.copy( item->szAccessPath, m_szAccessPath.length() );

	//item->szItemID = new char[m_szItemID.size()];
	//m_szItemID.copy( item->szItemID, m_szItemID.length() );

	item->szAccessPath = new char[m_szAccessPath.length() + 1];
	strcpy(item->szAccessPath, m_szAccessPath.c_str());
	item->szAccessPath[ m_szAccessPath.length() ] = 0;   // add null terminator

	item->szItemID = new char[m_szItemID.length() + 1];
	strcpy(item->szItemID, m_szItemID.c_str());
	item->szItemID[ m_szItemID.length() ] = 0;           // add null terminator

	return RET_OK;
}

long Item::IsDirty(bool * bDirtyFlag)
{
	if (m_BeenUpdated == false)
		*bDirtyFlag = false;
	else
		*bDirtyFlag = m_bDirty;

	m_bDirty = false;

	return RET_OK;
}

long Item::SetValue(const DATAITEM * data)
{
	long hr = RET_OK;

	if ( (isEqual(data, &m_diValue) == true ? false : true) )
	{
		m_bDirty = true;
	}

	if (m_bDirty == false && m_BeenUpdated == true)
	{
		return RET_OK;
	}
   
	m_BeenUpdated = true;

	// In this implementation, the canonical data type is meaningless. It can change
	// on the fly what format data for any ITEM is returned in. We must attempt to
	// coerce whatever data type just sent us to the "requested" data type that
	// was specified when this ITEM was added to the group in AddITEMs().   
	try
	{
		if (data->dt != m_diValue.dt && m_diValue.dt != DT_EMPTY && m_vtCanonicalDataType != DT_EMPTY )
		{
			// data types don't match; try to coerce.
			DATAITEM CrashDummy;
			DataItemInit(&CrashDummy);
			DataItemCopy(&CrashDummy, data);
			hr = DataItemChangeType(&CrashDummy, CrashDummy.dt);
			if (hr != RET_OK)
			{
				m_Quality = _BAD_QUALITY;
				throw hr;
			}
			else
			{
				DataItemClear(&CrashDummy);
				m_Quality = _GOOD_QUALITY;
				DataItemCopy(&m_diValue, data);
				if (hr != RET_OK)
				{
					m_Quality = _BAD_QUALITY;
					throw hr;
				}
			}
		}
		else 
		{
			DataItemCopy(&m_diValue, data);
			if (hr == RET_OK)
			{
				m_Quality = _GOOD_QUALITY;
			}
			else
			{
				m_Quality = _BAD_QUALITY;
			}
		}

		// We must set quality to BAD if the value of a
		// floating point number is NaN.
		if (m_vtCanonicalDataType == DT_R4)
		{
			if (ISNAN(m_diValue.fltVal) == 1)
			{
				m_Quality = _BAD_QUALITY;
			}
		}

		if (m_vtCanonicalDataType == DT_R8)
		{
			if (ISNAN(m_diValue.dblVal) == 1)
			{
				m_Quality = _BAD_QUALITY;
			}
		}
	}
	catch (long e)
	{
		hr = e;
	}

	time(&m_TimeStamp);

	return hr;
}

long Item::SetDataType(DATATYPE v)
{
	long hr = RET_OK;
   
	if( v != DT_EMPTY )
	{
		if( v != m_vtCanonicalDataType )
		{
			// Attempt to change to requested type
			DATAITEM data;
			DataItemInit( &data );
			hr = DataItemChangeType(&data, v);
			DataItemClear( &data );
		}
	}
	return hr;
}

long Item::GetValue(DATASOURCE ds, DATAITEM * data, unsigned short * qual, time_t * time)
{
	long hr = RET_OK;

	if ( qual != 0) 
	{
		switch(ds)
		{
			case DS_CACHE:
				*qual = m_bActive == true ? m_Quality : QUAL_NOTACTIVE;
				break;

			case DS_DEVICE:
			default:
   				*qual = m_Quality;
				break;
		}
	}

	if( time != 0 ) 
	{
		*time = m_TimeStamp;
	}

	if( data != 0 )
	{
		DataItemCopy(data, &m_diValue);
		if ((data->dt != DT_EMPTY) && (data->dt != m_vtCanonicalDataType))
		{
			hr = DataItemChangeType(data, m_vtCanonicalDataType);

			if ( hr !=RET_OK )
			{
				// This should never occur since m_vtRequested is only set after its
				// value is checked. 
				*qual = 0x00; // bad quality
			}
		}
	}

	return hr;
}

bool Item::isEqual(const DATAITEM * di1, const DATAITEM * di2)
{
	bool bRet = false;
   
	if ( di1->dt != di2->dt )
	{
		return false;
	}

	try
	{
		if ( ( di1->dt & DT_ARRAY ) != 0 || ( di2->dt & DT_ARRAY ) != 0 )
		{
			unsigned long nElements1, nElements2;

			DATAARRAY * psaVar1 = di1->parray;
			DATAARRAY * psaVar2 = di2->parray;

			DataArrayGetSize(psaVar1, &nElements1);
			DataArrayGetSize(psaVar2, &nElements2);
			if ( nElements1 != nElements2 )
				throw false;

			DATAITEM * pVarITEM1, * pVarITEM2;
			DataArrayLock( psaVar1 ); 
			DataArrayLock( psaVar2 ); 

			for(unsigned long i=0; i<nElements1; i++)
			{
				pVarITEM1 = (DATAITEM *)getElement(psaVar1, i);
				if(pVarITEM1==0) 
				{
					DataArrayUnlock( psaVar1 ); 
					DataArrayUnlock( psaVar2 ); 
					throw true;
				}

				pVarITEM2 = (DATAITEM *)getElement(psaVar2, i);
				if (pVarITEM2==0) 
				{
					DataArrayUnlock( psaVar1 ); 
					DataArrayUnlock( psaVar2 ); 
					throw true;
				}

				if((bRet = isEqual(pVarITEM1, pVarITEM2)) == false)
				{
					DataArrayUnlock( psaVar1 ); 
					DataArrayUnlock( psaVar2 ); 
					return bRet;
				}
			}
			DataArrayUnlock( psaVar1 ); 
			DataArrayUnlock( psaVar2 ); 
			return true;
		}
	}
	catch ( bool e )
	{
		return e;
	}

	switch ( di1->dt )
	{
	case DT_I4:
		bRet = ( di1->lVal == di2->lVal );
		break;

	case DT_UI1:
		bRet = ( di1->bVal == di2->bVal );
		break;

	case DT_I2:
		bRet = ( di1->iVal == di2->iVal );
		break;

	case DT_R4:
		bRet = ( di1->fltVal == di2->fltVal );
		break;

	case DT_R8:
		bRet = ( di1->dblVal == di2->dblVal );
		break;

	case DT_BOOL:
		bRet = ( di1->boolVal == di2->boolVal );
		break;

	case DT_DATE:
		bRet = difftime(di1->date, di2->date)==0;
		break;

	case DT_SSTR:
		bRet = strcmp( di1->sstringVal, di2->sstringVal ) == false ? true : false;
		break;

	case DT_I1:
		bRet = ( di1->cVal == di2->cVal );
		break;

	case DT_UI2:
		bRet = ( di1->uiVal == di2->uiVal );
		break;

	case DT_UI4:
		bRet = ( di1->ulVal == di2->ulVal );
		break;

	case DT_INT:
		bRet = ( di1->intVal == di2->intVal );
		break;

	case DT_UINT:
		bRet = ( di1->uintVal == di2->uintVal );
		break;

	default:
		break;
	}

	if ( bRet == false )
	{
		//"Value 1 Type is %i Value 2 type %i \n", di1->dt, di2->dt );
	}

	return bRet;
}

