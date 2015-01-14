#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataArray.h"
#include "DataItem.h"

ENTITYOBJECTS_API DATAARRAY * DataArray(DATATYPEENUM dt, unsigned long size)
{
	DATAARRAY * da = DataArrayAllocDescriptor();
	da->dt = dt;
	da->sizeOfElement = DataArrayGetSizeOfElement(da->dt);
	da->size = size;
	DataArrayAllocBuffer(da);
	return da;
}

ENTITYOBJECTS_API DATAARRAY * DataArray(const DATAARRAY * aCopy)
{
	DATAARRAY * da = DataArrayAllocDescriptor();
	da->dt = aCopy->dt;
    da->sizeOfElement = aCopy->sizeOfElement;
	da->size = aCopy->size;
	
	unsigned long bufferSize = DataArrayAllocBuffer(da);

	switch(da->dt)
	{
	case DT_EMPTY:
	case DT_NULL:
		break;

	case DT_I2:
	case DT_I4:
	case DT_R4:
	case DT_R8:
	case DT_ERROR:
	case DT_BOOL:
	case DT_DECIMAL:
	case DT_I1:
	case DT_UI1:
	case DT_UI2:
	case DT_UI4:
	case DT_I8:
	case DT_UI8:
	case DT_INT:
	case DT_UINT:
	case DT_VOID:
	case DT_LONG:
	case DT_DATE:
		memcpy(da->pvData, aCopy->pvData, bufferSize);
		break;

	case DT_SSTR:
	case DT_DATAITEM:
	case DT_ARRAY:
	{
		for(unsigned long i = 0; i < da->size; i++)
		{
			void * src = getElement(aCopy, i);
			void * dest = getElement(da, i);
			if(src==0 || dest==0)
				continue;

			switch(da->dt)
			{
			case DT_SSTR:
				dest = SafeString((SString)src);
				break;

			case DT_DATAITEM:
			{
				DataItemInit((DATAITEM *)dest);
				if(src!=0) 
				{
					DataItemCopy((DATAITEM *)dest, (DATAITEM *)src);
				}
				break;
			}

			case DT_ARRAY:
			{
				DataArrayCopy((DATAARRAY *)src, (DATAARRAY *)dest);
				break;
			}
			}
		}
		break;
	}

	case DT_PTR:
	case DT_LPSTR:
	case DT_INT_PTR:
	case DT_UINT_PTR:
	case DT_DOUBLE_PTR:
		break;
	}

	return da;
}

ENTITYOBJECTS_API DATAARRAY * DataArrayAllocDescriptor()
{
	DATAARRAY * da = new DATAARRAY;
	da->dt = DT_EMPTY;
    da->cLocks = 0;
	da->size = 0;
	da->sizeOfElement = 0;
	da->pvData = 0;

	return da;
}

ENTITYOBJECTS_API unsigned long DataArrayAllocBuffer(DATAARRAY * da)
{
	unsigned long bufferSize = 0;
	if(da->dt!=DT_EMPTY && da->size>0) 
	{
		unsigned long bufferSize = da->size * (da->sizeOfElement+1);
		if(bufferSize>0)
		{
			da->pvData = new unsigned char[bufferSize];
			memset(da->pvData, 0x00, bufferSize);
		}
	}
	return bufferSize;
}

ENTITYOBJECTS_API void DataArrayCopy(const DATAARRAY * aCopy, DATAARRAY * da)
{
	DataArrayFree(da);
	da->dt = aCopy->dt;
    da->sizeOfElement = aCopy->dt;
	da->size = aCopy->size;
    da->cLocks = 0;
	da->pvData = 0;

	unsigned long size = DataArrayAllocBuffer(da);

	switch(da->dt)
	{
	case DT_EMPTY:
	case DT_NULL:
		break;

	case DT_I2:
	case DT_I4:
	case DT_R4:
	case DT_R8:
	case DT_ERROR:
	case DT_BOOL:
	case DT_DECIMAL:
	case DT_I1:
	case DT_UI1:
	case DT_UI2:
	case DT_UI4:
	case DT_I8:
	case DT_UI8:
	case DT_INT:
	case DT_UINT:
	case DT_VOID:
	case DT_LONG:
	case DT_DATE:
		memcpy(da->pvData, aCopy->pvData, size);
		break;

	case DT_SSTR:
	case DT_DATAITEM:
	case DT_ARRAY:
	{
		for(unsigned long i = 0; i < da->size; i++)
		{
			void * src =getElement(aCopy, i);
			void * dest = getElement(da, i);
			if(src==0 || dest==0)
				continue;

			switch(da->dt)
			{
			case DT_SSTR:
				dest = SafeString((SString)src);
				break;

			case DT_DATAITEM:
				DataItemInit((DATAITEM *)dest);
				if(src!=0) 
				{
					DataItemCopy((DATAITEM *)dest, (DATAITEM *)src);
				}
				break;

			case DT_ARRAY:
				DataArrayCopy((DATAARRAY *)src, (DATAARRAY *)dest );
				break;
			}
		}
		break;
	}
	case DT_PTR:
	case DT_LPSTR:
	case DT_INT_PTR:
	case DT_UINT_PTR:
	case DT_DOUBLE_PTR:
		break;
	}
}

ENTITYOBJECTS_API void DataArrayFree(DATAARRAY * da)
{
	if (da != nullptr) 
	{
		if (da->dt != DT_EMPTY && da->pvData != 0)
		{
			for (unsigned long i = 0; i < da->size; i++)
			{
				void * src = getElement(da, i);
				if (src == 0)
					continue;

				switch (da->dt)
				{
				case DT_SSTR:
					SafeStringFree((SString)src);
					break;

				case DT_DATAITEM:
					DataItemClear((DATAITEM *)src);
					break;

				case DT_ARRAY:
					DataArrayFree((DATAARRAY *)src);
					break;

				case DT_PTR:
				case DT_LPSTR:
				case DT_INT_PTR:
				case DT_UINT_PTR:
				case DT_DOUBLE_PTR:
					delete src;
					break;
				}
			}

			delete da->pvData;
		}
		da->pvData = 0;
		da->dt = DT_EMPTY;
		da->size = 0;
	}
}

ENTITYOBJECTS_API void DataArrayDestroyDescriptor(DATAARRAY * da)
{
	delete da;
}

ENTITYOBJECTS_API unsigned long DataArrayGetSizeOfElement(DATATYPE dt)
{
	unsigned long size = 0;

	switch(dt)
	{
	case DT_EMPTY:
	case DT_NULL:
		break;
	case DT_I2:
		size = sizeof(short);
		break;
	case DT_I4:
		size = sizeof(int);
		break;
	case DT_R4:
		size = sizeof(float);
		break;
	case DT_R8:
		size = sizeof(double);
		break;
	case DT_DATE:
		size = sizeof(double);
		break;
	case DT_SSTR:
		size = sizeof(char *);
		break;
	case DT_ERROR:
		size = sizeof(long);
		break;
	case DT_BOOL:
		size = sizeof(int);
		break;
	case DT_DATAITEM:
		size = sizeof(DATAITEM);
		break;
	case DT_DECIMAL:
		size = sizeof(DECIMAL);
		break;
	case DT_I1:
		size = sizeof(char);
		break;
	case DT_UI1:
		size = sizeof(unsigned char);
		break;
	case DT_UI2:
		size = sizeof(unsigned int);
		break;
	case DT_UI4:
		size = sizeof(unsigned long);
		break;
	case DT_I8:
		size = sizeof(INT64);
		break;
	case DT_UI8:
		size = sizeof(UINT64);
		break;
	case DT_INT:
		size = sizeof(int);
		break;
	case DT_UINT:
		size = sizeof(unsigned int);
		break;
	case DT_VOID:
		size = sizeof(void *);
		break;
	case DT_LONG:
		size = sizeof(long);
		break;
	case DT_ARRAY:
		size = sizeof(DATAARRAY);
		break;
	case DT_BYREF:
	case DT_PTR:
	case DT_LPSTR:
	case DT_INT_PTR:
	case DT_UINT_PTR:
	case DT_DOUBLE_PTR:
	case DT_VECTOR:
	default:
		size = sizeof(void *);
		break;
	}

	return size;
}

ENTITYOBJECTS_API void DataArrayLock(DATAARRAY * da)
{
	da->cLocks++;
}

ENTITYOBJECTS_API void DataArrayUnlock(DATAARRAY * da)
{
	da->cLocks--;
}

ENTITYOBJECTS_API void DataArrayAccessData(DATAARRAY * da, void ** ppvData)
{
	 DataArrayLock(da);
	 *ppvData = da->pvData;
}

ENTITYOBJECTS_API void DataArrayUnaccessData(DATAARRAY * da)
{
	 DataArrayUnlock(da);
}

ENTITYOBJECTS_API void DataArrayGetElement(DATAARRAY * da, long idx, void * pv)
{
	void * src = getElement(da, idx);
	if(src!=0)
	{
		switch(da->dt)
		{
		case DT_EMPTY:
		case DT_NULL:
			break;

		case DT_I2:
		case DT_I4:
		case DT_R4:
		case DT_R8:
		case DT_ERROR:
		case DT_BOOL:
		case DT_DECIMAL:
		case DT_I1:
		case DT_UI1:
		case DT_UI2:
		case DT_UI4:
		case DT_I8:
		case DT_UI8:
		case DT_INT:
		case DT_UINT:
		case DT_VOID:
		case DT_LONG:
		case DT_DATE:
			memcpy(pv, src, da->sizeOfElement);
			break;

		case DT_SSTR:
			pv = SafeString((SString)src);
			break;

		case DT_DATAITEM:
			DataItemCopy((DATAITEM *)pv, (DATAITEM *)src);
			break;

		case DT_ARRAY:
			DataArrayCopy((DATAARRAY*)src, (DATAARRAY *)pv);
			break;

		case DT_PTR:
		case DT_LPSTR:
		case DT_INT_PTR:
		case DT_UINT_PTR:
		case DT_DOUBLE_PTR:
		default:
			memcpy(pv, src, sizeof(void*));
			break;
		}
	}
}

ENTITYOBJECTS_API long DataArrayPutElement(DATAARRAY * da, long idx, void * pv)
{
	if(da->cLocks!=0)
		return RET_ERROR;

	void * dest = getElement(da, idx);
	if(dest==0)
		return RET_ERROR;

	switch(da->dt)
	{
	case DT_EMPTY:
	case DT_NULL:
		break;

	case DT_I2:
	case DT_I4:
	case DT_R4:
	case DT_R8:
	case DT_ERROR:
	case DT_BOOL:
	case DT_DECIMAL:
	case DT_I1:
	case DT_UI1:
	case DT_UI2:
	case DT_UI4:
	case DT_I8:
	case DT_UI8:
	case DT_INT:
	case DT_UINT:
	case DT_VOID:
	case DT_LONG:
	case DT_DATE:
		memcpy(dest, pv, da->sizeOfElement);
		break;

	case DT_SSTR:
		dest = SafeString((SString)pv);
		break;

	case DT_DATAITEM:
		DataItemCopy((DATAITEM *)dest, (const DATAITEM *)pv);
		break;

	case DT_ARRAY:
		DataArrayCopy((DATAARRAY*)pv, (DATAARRAY*)dest);
		break;

	case DT_PTR:
	case DT_LPSTR:
	case DT_INT_PTR:
	case DT_UINT_PTR:
	case DT_DOUBLE_PTR:
	default:
		memcpy(dest, pv, sizeof(void*));
		break;
	}

	return RET_OK;
}

ENTITYOBJECTS_API void DataArrayGetDatatype(DATAARRAY * da, DATATYPE * pvt)
{
	*pvt=da->dt;
}

ENTITYOBJECTS_API void DataArrayGetSize(DATAARRAY * da, unsigned long * lElements)
{
	*lElements = da->size;
}

ENTITYOBJECTS_API char * DataArraytoString(DATAARRAY * da)
{
	char * buffer = 0;
	return buffer;
}

ENTITYOBJECTS_API int DataArrayMarshal(unsigned char * dest, DATAARRAY * da)
{
	long bytesUsed = 0;
      
	memcpy(dest, da, sizeof(DATAARRAY));
	dest += sizeof(DATAARRAY);
	bytesUsed += sizeof(DATAARRAY);

	long sizeofData = 0;
	if(da->dt == DT_SSTR)
	{
		for(unsigned long i = 0; i < da->size; i++)
		{
			void * src = getElement(da, i);
			if(src!=0)
			{
				sizeofData = SafeStringWriteImage(dest, (SString)src);
				dest += sizeofData;
				bytesUsed += sizeofData;
			}
		}
	}
	else if(da->dt == DT_DATAITEM)
	{
		for(unsigned long i = 0; i < da->size; i++)
		{
			void * src = getElement(da, i);
			if(src!=0)
			{
				sizeofData = DataItemMarshal(dest, (DATAITEM *)src);
				dest += sizeofData;
				bytesUsed += sizeofData;
			}
		}
	}
	else
	{
		// Write the data from the source SA to the storage medium
		sizeofData = da->sizeOfElement * da->size;
		if(sizeofData>0)
		{
			memcpy(dest, da->pvData, sizeofData);
			bytesUsed += sizeofData;
		}
	} 

	return bytesUsed;
}

ENTITYOBJECTS_API int DataArrayUnMarshal(unsigned char * src, DATAARRAY * da)
{
	long bytesUsed = 0;
	unsigned long bufferSize = 0;

	memcpy(da, src, sizeof(DATAARRAY));
	src += sizeof(DATAARRAY);
	bytesUsed += sizeof(DATAARRAY);
	da->pvData = 0;
      
	if(da->dt!=DT_EMPTY) 
	{
		da->sizeOfElement = DataArrayGetSizeOfElement(da->dt);
		if(da->size>0) 
		{
			bufferSize = da->size * da->sizeOfElement;
			if(bufferSize>0)
			{
				da->pvData = new unsigned char[bufferSize];
				memset(da->pvData, 0x00, bufferSize);
			}
		}
	}
      
	long sizeOfData = 0;
	void * dest = 0;
	if(da->dt == DT_SSTR)
	{
		for(unsigned long i = 0; i < da->size; i++)
		{
			dest = getElement(da, i);
			if(dest!=0)
			{
				sizeOfData = SafeStringReadImage(src, (SString *)dest);
				src += sizeOfData;
				bytesUsed += sizeOfData;
			}
		}
	}
	else if(da->dt == DT_DATAITEM)
	{
		for(unsigned long i = 0; i < da->size; i++)
		{
			dest = getElement(da, i);
			if(dest!=0)
			{
				sizeOfData = DataItemUnMarshal(src, (DATAITEM *)dest);
				src += sizeOfData;
				bytesUsed += sizeOfData;
			}
		}
	}
	else
	{
		if(bufferSize>0)
		{
			memcpy(da->pvData, src, bufferSize);
			bytesUsed += bufferSize;
		}
	}

	return bytesUsed;
}

ENTITYOBJECTS_API unsigned long DataArrayGetSize(DATAARRAY * da)
{
	unsigned long arraySize = sizeof(DATAARRAY);
	
	if(da==0)
	{
		return arraySize;
	}

	void * src = 0;
	if(da->dt == DT_SSTR)
	{      
		for(unsigned long i = 0; i < da->size; i++)
		{
			src = getElement(da, i);
			if(src!=0)
			{
				arraySize += SafeStringByteLen((SString)src);
			}
		}
	}
	else if(da->dt == DT_DATAITEM)
	{      
		for(unsigned long i = 0; i < da->size; i++)
		{
			src = getElement(da, i);
			if(src!=0)
			{
				arraySize += DataItemGetItemSize((DATAITEM *)src);
			}
		}
	}
	else if(da->dt == DT_ARRAY)
	{      
		for(unsigned long i = 0; i < da->size; i++)
		{
			src = getElement(da, i);
			if(src!=0)
			{
				arraySize += DataArrayGetSize((DATAARRAY *)src);
			}
		}
	}
	else
	{
		int size = 0;
		switch(da->dt)
		{
		case DT_EMPTY:
		case DT_NULL:
			break;
		case DT_I2:
			size = sizeof(short);
			break;
		case DT_I4:
			size = sizeof(int);
			break;
		case DT_R4:
			size = sizeof(float);
			break;
		case DT_R8:
			size = sizeof(double);
			break;
		case DT_DATE:
			size = sizeof(double);
			break;
		case DT_SSTR:
			size = sizeof(char *);
			break;
		case DT_ERROR:
			size = sizeof(long);
			break;
		case DT_BOOL:
			size = sizeof(int);
			break;
		case DT_DECIMAL:
			size = sizeof(DECIMAL);
			break;
		case DT_I1:
			size = sizeof(char);
			break;
		case DT_UI1:
			size = sizeof(unsigned char);
			break;
		case DT_UI2:
			size = sizeof(unsigned int);
			break;
		case DT_UI4:
			size = sizeof(unsigned long);
			break;
		case DT_I8:
			size = sizeof(INT64);
			break;
		case DT_UI8:
			size = sizeof(UINT64);
			break;
		case DT_INT:
			size = sizeof(int);
			break;
		case DT_UINT:
			size = sizeof(unsigned int);
			break;
		case DT_VOID:
			size = sizeof(void *);
			break;
		case DT_LONG:
			size = sizeof(long);
			break;
		case DT_PTR:
		case DT_LPSTR:
		case DT_INT_PTR:
		case DT_UINT_PTR:
		case DT_DOUBLE_PTR:
		case DT_VECTOR:
		case DT_ARRAY:
		default:
			size = sizeof(void *);
			break;
		}
		size *= da->size;
		arraySize += size;
	}
	return arraySize;
}
