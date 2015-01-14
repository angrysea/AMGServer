#pragma once
#include "export.h"
#include "types.h"
#include "SafeString.h"

typedef struct ENTITYOBJECTS_API tagDATAARRAY
{
    unsigned long sizeOfElement;
    unsigned long size;
    unsigned long cLocks;
	DATATYPEENUM dt;
    void * pvData;
} DATAARRAY;

ENTITYOBJECTS_API DATAARRAY * DataArray(DATATYPEENUM dt, unsigned long size);
ENTITYOBJECTS_API DATAARRAY * DataArray(const DATAARRAY * aCopy);
ENTITYOBJECTS_API DATAARRAY * DataArrayAllocDescriptor();
ENTITYOBJECTS_API void DataArrayCopy(const DATAARRAY * aCopy, DATAARRAY * da);
ENTITYOBJECTS_API void DataArrayFree(DATAARRAY * da);
ENTITYOBJECTS_API void DataArrayDestroyDescriptor(DATAARRAY * da);
ENTITYOBJECTS_API unsigned long DataArrayAllocBuffer(DATAARRAY * da);
ENTITYOBJECTS_API void Redim(DATAARRAY * da, unsigned long size);
ENTITYOBJECTS_API void DataArrayGetSize(DATAARRAY * da, unsigned long * lElements);
ENTITYOBJECTS_API unsigned long DataArrayGetSizeOfElement(DATATYPE dt);
ENTITYOBJECTS_API void DataArrayLock(DATAARRAY * da);
ENTITYOBJECTS_API void DataArrayUnlock(DATAARRAY * da);
ENTITYOBJECTS_API void DataArrayAccessData(DATAARRAY * da, void ** ppvData);
ENTITYOBJECTS_API void DataArrayUnaccessData(DATAARRAY * da);
ENTITYOBJECTS_API void DataArrayGetElement(DATAARRAY * da, long idx, void * pv);
ENTITYOBJECTS_API long DataArrayPutElement(DATAARRAY * da, long idx, void * pv);
ENTITYOBJECTS_API void DataArrayGetDatatype(DATAARRAY * da, DATATYPE * pvt);
ENTITYOBJECTS_API int DataArrayMarshal(unsigned char * dest, DATAARRAY * da);
ENTITYOBJECTS_API int DataArrayUnMarshal(unsigned char * dest, DATAARRAY * da);
ENTITYOBJECTS_API unsigned long DataArrayGetSize(DATAARRAY * da);
ENTITYOBJECTS_API char * DataArraytoString(DATAARRAY * da);

inline ENTITYOBJECTS_API void * getElement(const DATAARRAY * pDA, unsigned long idx)
{ 
	if(pDA->pvData!=0 && idx<pDA->size)
	{
		return ((unsigned char *)pDA->pvData) + (pDA->sizeOfElement * idx); 
	}
	else
	{
		return 0;
	}
}
