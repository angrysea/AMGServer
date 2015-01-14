#pragma once
#include <string.h>

#include "rtds.h"

class _CopyUMSITEMATTRIBUTES
{
public:
	
	static long copy(ITEMATTRIBUTES** pp1, ITEMATTRIBUTES** pp2) 
	{
		ITEMATTRIBUTES* p1 = *pp1;
		ITEMATTRIBUTES* p2 = *pp2; 
		p1->szAccessPath = new char[strlen( p2->szAccessPath )];
		strcpy(p1->szAccessPath, p2->szAccessPath);

		p1->szItemID = new char[strlen( p2->szItemID )];
		strcpy(p1->szItemID, p2->szItemID);
       
		p1->bActive = p2->bActive;
		p1->hClient = p2->hClient;
		p1->hServer = p2->hServer;
		p1->dwAccessRights = p2->dwAccessRights;
		p1->dwBlobSize = p2->dwBlobSize;
		p1->pBlob = p2->pBlob;
		p1->vtRequestedDataType = p2->vtRequestedDataType;
		p1->vtCanonicalDataType = p2->vtCanonicalDataType;
		p1->dwEUType = p2->dwEUType;
		DataItemInit(p1->vEUInfo);
		DataItemCopy(p1->vEUInfo, p2->vEUInfo);

		return RET_OK;
	}

	static void init(ITEMATTRIBUTES** p) {*p = NULL;}
	
	static void destroy(ITEMATTRIBUTES** pp ) 
	{
		ITEMATTRIBUTES * p = *pp;
		delete [] p->szAccessPath;
		delete [] p->szItemID;
		DataItemClear(p->vEUInfo);
		delete p->vEUInfo;
	}
};
