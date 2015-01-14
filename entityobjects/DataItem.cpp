#include <stdio.h>
#include <string.h>
#include "DataItem.h"

ENTITYOBJECTS_API void DataItemInit(DATAITEM * di)
{
	di->dt = DT_EMPTY;
	DECIMAL_SETZERO(di->decVal);
	di->byref = 0;
}

ENTITYOBJECTS_API void DataItemClear(DATAITEM * di)
{
	if(di!=0 && di->dt != DT_EMPTY)
	{
		if( (di->dt & DT_ARRAY ) == DT_ARRAY )
		{
			if(di->parray!=0) 
			{
				DataArrayFree(di->parray);
				di->parray = 0;
				di->dt = DT_EMPTY;
			}
		}
		else
		{
			switch(di->dt)
			{
			case DT_SSTR:
				if(di->sstringVal!=0) 
				{
					SafeStringFree(di->sstringVal);
				}
				break;
			case DT_PTR:
				if(di->byref!=0) 
				{
					delete di->byref;
				}
				break;
			case DT_LPSTR:
				if(di->pcVal!=0)
				{
					delete di->pcVal;
				}
				break;
			case DT_INT_PTR:
				if(di->pintVal!=0)
				{
					delete di->pintVal;
				}
				break;
			case DT_UINT_PTR:
				if(di->puintVal!=0)
				{
					delete di->puintVal;
				}
				break;
			case DT_DECIMAL:
				DECIMAL_SETZERO(di->decVal);
				break;
			//case DT_FILETIME:
			//	break;
			case DT_BYREF:
				break;
			default:
				break;
			}
		}
		di->dt = DT_EMPTY;
		di->byref = 0;
	}
}

ENTITYOBJECTS_API bool DataItemChangeType(DATAITEM * source, DATATYPE dt)
{

	if(source->dt==dt)
	{
		return true;
	}

	if((source->dt & DT_ARRAY ) == DT_ARRAY) 
	{
		DATAARRAY * da = source->parray;
		if(da->dt==dt)
		{
			return true;
		}
	}

	switch(source->dt)
	{
	case DT_EMPTY:
	case DT_NULL:
		break;
	case DT_I2:
		break;
	case DT_I4:
		break;
	case DT_R4:
		break;
	case DT_R8:
		break;
	case DT_DATE:
		break;
	case DT_SSTR:
		break;
	case DT_ERROR:
		break;
	case DT_BOOL:
		break;
	case DT_DATAITEM:
		break;
	case DT_DECIMAL:
		break;
	case DT_I1:
		break;
	case DT_UI1:
		break;
	case DT_UI2:
		break;
	case DT_UI4:
		break;
	case DT_I8:
		break;
	case DT_UI8:
		break;
	case DT_INT:
		break;
	case DT_UINT:
		break;
	case DT_VOID:
		break;
	case DT_LONG:
		break;
	case DT_PTR:
		break;
	case DT_LPSTR:
		break;
	case DT_INT_PTR:
		break;
	case DT_UINT_PTR:
		break;
	//case DT_FILETIME:
	//	break;
	default:
		break;
	}

	return true;
}

ENTITYOBJECTS_API void DataItemCopy(DATAITEM * dest, const DATAITEM * source)
{
	DataItemClear(dest);
	dest->dt = source->dt;

	if( (source->dt & DT_ARRAY ) == DT_ARRAY )
	{
		 dest->parray = DataArray(source->parray);
	}
	else
	{
		switch(source->dt)
		{
		case DT_EMPTY:
		case DT_NULL:
			break;
		case DT_I2:
			dest->iVal=source->iVal;
			break;
		case DT_I4:
			dest->intVal=source->intVal;
			break;
		case DT_R4:
			dest->fltVal=source->fltVal;
			break;
		case DT_R8:
			dest->dblVal=source->dblVal;
			break;
		case DT_DATE:
			dest->date=source->date;
			break;
		case DT_SSTR:
			dest->sstringVal = SafeString(source->sstringVal);
			break;
		case DT_ERROR:
			dest->scode=source->scode;
			break;
		case DT_BOOL:
			dest->boolVal=source->boolVal;
			break;
		case DT_DATAITEM:
			dest->pvarData = source->pvarData;
			break;
		case DT_DECIMAL:
			dest->pdecVal = source->pdecVal;
			break;
		case DT_I1:
			dest->cVal=source->cVal;
			break;
		case DT_UI1:
			dest->bVal=source->bVal;
			break;
		case DT_UI2:
			dest->uintVal=source->uintVal;
			break;
		case DT_UI4:
			dest->ulVal=source->ulVal;
			break;
		case DT_I8:
			dest->llVal=source->llVal;
			break;
		case DT_UI8:
			dest->ullVal=source->ullVal;
			break;
		case DT_INT:
			dest->intVal=source->intVal;
			break;
		case DT_UINT:
			dest->uintVal=source->uintVal;
			break;
		case DT_LONG:
			dest->scode=source->scode;
			break;
		case DT_PTR:
			break;
		case DT_LPSTR:
			strcpy(dest->pcVal, source->pcVal);
			break;
		case DT_INT_PTR:
			dest->pintVal = source->pintVal;
			break;
		case DT_UINT_PTR:
			if(source->puintVal!=0)
			{
				delete source->puintVal;
			}
			dest->puintVal = source->puintVal;
			break;
		//case DT_FILETIME:
		//	break;
		default:
			break;
		}
	}	
}

ENTITYOBJECTS_API void DataItemMove(DATAITEM * dest, DATAITEM * source)
{
	DataItemClear(dest);
	dest->dt = source->dt;

	if ((source->dt & DT_ARRAY) == DT_ARRAY)
	{
		dest->parray = source->parray;
		source->parray = 0;
	}
	else
	{
		switch (source->dt)
		{
		case DT_EMPTY:
		case DT_NULL:
			break;
		case DT_I2:
			dest->iVal = source->iVal;
			break;
		case DT_I4:
			dest->intVal = source->intVal;
			break;
		case DT_R4:
			dest->fltVal = source->fltVal;
			break;
		case DT_R8:
			dest->dblVal = source->dblVal;
			break;
		case DT_DATE:
			dest->date = source->date;
			break;
		case DT_SSTR:
			dest->sstringVal = source->sstringVal;
			source->sstringVal = nullptr;
			break;
		case DT_ERROR:
			dest->scode = source->scode;
			break;
		case DT_BOOL:
			dest->boolVal = source->boolVal;
			break;
		case DT_DATAITEM:
			dest->pvarData = source->pvarData;
			break;
		case DT_DECIMAL:
			dest->pdecVal = source->pdecVal;
			break;
		case DT_I1:
			dest->cVal = source->cVal;
			break;
		case DT_UI1:
			dest->bVal = source->bVal;
			break;
		case DT_UI2:
			dest->uintVal = source->uintVal;
			break;
		case DT_UI4:
			dest->ulVal = source->ulVal;
			break;
		case DT_I8:
			dest->llVal = source->llVal;
			break;
		case DT_UI8:
			dest->ullVal = source->ullVal;
			break;
		case DT_INT:
			dest->intVal = source->intVal;
			break;
		case DT_UINT:
			dest->uintVal = source->uintVal;
			break;
		case DT_LONG:
			dest->scode = source->scode;
			break;
		case DT_PTR:
			break;
		case DT_LPSTR:
			dest->pcVal=source->pcVal;
			source->pcVal = nullptr;
			break;
		case DT_INT_PTR:
			dest->pintVal = source->pintVal;
			break;
		case DT_UINT_PTR:
			dest->puintVal = source->puintVal;
			source->puintVal = nullptr;
			break;
			//case DT_FILETIME:
			//	break;
		default:
			break;
		}
	}
}

ENTITYOBJECTS_API unsigned long DataItemGetItemSize(DATAITEM * di)
{
	unsigned long size = sizeof(DATAITEM);

	if(di!=0)
	{
		if((di->dt & DT_ARRAY) == DT_ARRAY)
		{
			size += DataArrayGetSize(di->parray);
		}
		else if(di->dt==DT_SSTR)
		{
			size += SafeStringByteLen(di->sstringVal);
		}
	}

	return size;
}

/*
 * Writes the data contained in the variant pData to the buffer dest.
 * Returns the number of bytes written to buffer.
 * Note: in certain cases this method is recursive.
 */
ENTITYOBJECTS_API int DataItemMarshal(unsigned char * dest, DATAITEM * pData)
{
	int bytesUsed = sizeof(DATAITEM);
	memcpy(dest, pData, sizeof(DATAITEM));
	dest += sizeof(DATAITEM);

	if( pData->dt == DT_SSTR )
	{
		bytesUsed += SafeStringWriteImage(dest, pData->sstringVal);
	}
	else if((pData->dt & DT_ARRAY) == DT_ARRAY)
	{
		bytesUsed += DataArrayMarshal(dest, pData->parray);
	}

	return bytesUsed;
}

ENTITYOBJECTS_API int DataItemUnMarshal(unsigned char * src, DATAITEM * pData)
{
	int bytesUsed = sizeof(DATAITEM);
	memcpy(pData, src, sizeof(DATAITEM));
	src += sizeof(DATAITEM);

	if( (pData)->dt == DT_SSTR )
	{
		bytesUsed += SafeStringReadImage(src, &(pData->sstringVal));
	}
	else if((pData->dt & DT_ARRAY) == DT_ARRAY)
	{
		pData->parray = DataArrayAllocDescriptor();
		bytesUsed += DataArrayUnMarshal(src, pData->parray);
	}

	return bytesUsed;
}

#define BUF_SIZE 100
ENTITYOBJECTS_API char * DataItemtoString(DATAITEM * di)
{
	char * buffer = 0;
	if((di->dt & DT_ARRAY ) == DT_ARRAY) 
	{
		buffer = DataArraytoString(di->parray);
	}
	else if(di->dt==DT_SSTR && di->sstringVal !=0 ) 
	{
		buffer = SafeStringToString(di->sstringVal);
	}
	else if(di->dt==DT_LPSTR)
	{
		int size = strlen(di->pcVal);
		buffer = new char[size+1];
		strcpy(buffer, di->pcVal);
	}
	else
	{
		buffer = new char[BUF_SIZE+1];
		memset(buffer, 0x00, BUF_SIZE+1);
		switch(di->dt)
		{
		case DT_EMPTY:
		case DT_NULL:
			break;
		case DT_I2:
			sprintf(buffer, "%d", di->iVal);
			break;
		case DT_I4:
			sprintf(buffer, "%d", di->lVal);
			break;
		case DT_R4:
			sprintf(buffer, "%8.6f", di->fltVal);
			break;
		case DT_R8:
			sprintf(buffer, "%12.6f", di->dblVal);
			break;
		case DT_DATE:
			break;
		case DT_ERROR:
			sprintf(buffer, "%d", di->scode);
			break;
		case DT_BOOL:
			strcpy(buffer, di->boolVal ? "TRUE" : "FALSE");
			break;
		case DT_DECIMAL:
			sprintf(buffer, "%12.6f", di->decVal);
			break;
		case DT_I1:
			sprintf(buffer, "%d", di->bVal);
			break;
		case DT_UI1:
			sprintf(buffer, "%u", di->uiVal);
			break;
		case DT_UI2:
			sprintf(buffer, "%u", di->uiVal);
			break;
		case DT_UI4:
			sprintf(buffer, "%u", di->ulVal);
			break;
		case DT_I8:
			sprintf(buffer, "%d", di->llVal);
			break;
		case DT_UI8:
			sprintf(buffer, "%d", di->ullVal);
			break;
		case DT_INT:
			sprintf(buffer, "%d", di->intVal);
			break;
		case DT_UINT:
			sprintf(buffer, "%d", di->uintVal);
			break;
		case DT_VOID:
			break;
		case DT_LONG:
			sprintf(buffer, "%d", di->lVal);
			break;
		case DT_PTR:
			break;
		case DT_INT_PTR:
			break;
		case DT_UINT_PTR:
			break;
		//case DT_FILETIME:
		//	break;
		default:
			break;
		}
	}
	return buffer;
}

