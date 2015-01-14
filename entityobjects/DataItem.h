#pragma once
#include <time.h>

#include "export.h"
#include "types.h"
#include "DataArray.h"
#include "SafeString.h"

typedef struct tagDATAITEM DATAITEM;

struct ENTITYOBJECTS_API tagDATAITEM
{
    union 
        {
        struct 
            {
            DATATYPE dt;
            union 
			{
                INT64 llVal;
                long lVal;
                unsigned char bVal;
                short iVal;
                float fltVal;
                double dblVal;
                bool boolVal;
                time_t date;
                SString sstringVal;
                DATAARRAY * parray;
                long scode;
                unsigned char * pbVal;
                short * piVal;
                long * plVal;
                INT64 * pllVal;
                float * pfltVal;
                double * pdblVal;
                bool * pboolVal;
                long * pscode;
                INT64 * pcyVal;
                time_t * pdate;
                SString * psstringVal;
                DATAARRAY ** pparray;
                DATAITEM * pvarData;
                void * byref;
                char cVal;
                unsigned short uiVal;
                unsigned long ulVal;
                UINT64 ullVal;
                int intVal;
                unsigned int uintVal;
                DECIMAL * pdecVal;
                char * pcVal;
                unsigned short * puiVal;
                unsigned long * pulVal;
                UINT64 * pullVal;
                int * pintVal;
                unsigned int * puintVal;
            };
		};
		DECIMAL decVal;
    };
};

ENTITYOBJECTS_API void DataItemInit(DATAITEM * di);
ENTITYOBJECTS_API void DataItemClear(DATAITEM * di);
ENTITYOBJECTS_API bool DataItemChangeType(DATAITEM * pSource, DATATYPE dt);
ENTITYOBJECTS_API unsigned long DataItemGetItemSize(DATAITEM * di);
ENTITYOBJECTS_API void DataItemCopy(DATAITEM * dest, const DATAITEM * source);
ENTITYOBJECTS_API void DataItemMove(DATAITEM * dest, const DATAITEM * source);
ENTITYOBJECTS_API int DataItemMarshal(unsigned char * dest, DATAITEM * pData);
ENTITYOBJECTS_API int DataItemUnMarshal(unsigned char * src, DATAITEM * pData);
ENTITYOBJECTS_API char * DataItemtoString(DATAITEM * source);
