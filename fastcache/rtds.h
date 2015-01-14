#pragma once

// Server
// -classpath C:\projects\adaptinet\sdk\target\classes;C:\projects\adaptinet\lib\jtds-1.2.4.jar;C:\Users\grafant\AppData\.m2\repository\log4j\log4j\1.2.16\log4j-1.2.16.jar;C:\projects\adaptinet\fastcachej\target;C:\projects\adaptinet\InstrumentCache\bin -Djava.library.path=C:\projects\adaptinet\fastcachej\fastcache\Debug org.adaptinet.transceiver.Transceiver -config C:/projects/adaptinet/InstrumentCache/conf/transceiver1.properties
// Client
// -classpath C:\projects\adaptinet\sdk\target\classes;C:\projects\adaptinet\instrumentclient\bin;C:\projects\adaptinet\fastcachej\target;C:\projects\adaptinet\InstrumentCache\bin -Djava.library.path=C:\projects\adaptinet\fastcachej\fastcache\Debug com.db.instrument.client.Client

#include <time.h>
#include "types.h"
#include "DataItem.h"

#define QUAL_NOTACTIVE 0x1c
#define _GOOD_QUALITY 0xc0
#define _BAD_QUALITY 0x00
#define _UNCERTAIN_QUALITY 0x40

#define DT_DT(X)         ((X)->dt)
#define DT_ARRAY(X)       ((X)->parray)

enum EUTYPE {NOENUM, ANALOG, ENUMERATED};
enum ACCESSRIGHTS {READABLE, WRITEABLE};
enum DATASOURCE {DS_CACHE, DS_DEVICE};
enum SERVERSTATE {STATUS_RUNNING = 1, STATUS_FAILED, STATUS_NOCONFIG,
					STATUS_SUSPENDED, STATUS_TEST};
enum BROWSEDIRECTION {BROWSE_UP = 1, BROWSE_DOWN};
enum ENUMSCOPE {ENUM_PRIVATE_CONNECTIONS	= 1, ENUM_PUBLIC_CONNECTIONS,
				ENUM_ALL_CONNECTIONS, ENUM_PRIVATE, ENUM_PUBLIC, ENUM_ALL};

typedef struct __GROUPHEADER {
	unsigned long dwSize;
	unsigned long dwItemCount;
	long hClientGroup;
	unsigned long dwTransactionID;
	long hrStatus;
} GROUPHEADER;

typedef struct __ITEMHEADER1 {
	long hClient;
	unsigned long dwValueOffset;
	unsigned short wQuality;
	unsigned short wReserved;
	time_t ftTimeStampItem;
} ITEMHEADER1;

typedef struct __ITEMHEADER2 {
    long hClient;
    unsigned long dwValueOffset;
    unsigned short wQuality;
    unsigned short wReserved;
} ITEMHEADER2;

typedef struct __GROUPHEADERWRITE {
    unsigned long dwItemCount;
    long hClientGroup;
    unsigned long dwTransactionID;
    long hrStatus;
} GROUPHEADERWRITE;

typedef struct __ITEMHEADERWRITE {
    long hClient;
    long dwError;
} ITEMHEADERWRITE;

typedef struct __ITEMSTATE {
	long hClient;
	time_t ftTimeStamp;
	unsigned short wQuality;
	unsigned short wReserved;
	DATAITEM vDataValue;
} ITEMSTATE;

typedef struct __SERVERSTATUS {
	time_t ftStartTime;
	time_t ftCurrentTime;
	time_t ftLastUpdateTime;
	SERVERSTATE dwServerState;
	unsigned long dwGroupCount; 
	unsigned long dwBandWidth;
	unsigned short wMajorVersion;
	unsigned short wMinorVersion;
	unsigned short wBuildNumber;
	unsigned short wReserved;
	char * szVendorInfo;
} SERVERSTATUS;

typedef struct __ITEMDEF {
	char * szAccessPath;
	char * szItemID;
	bool bActive ;
	long hClient;
	unsigned long dwBlobSize;
	char * pBlob;
	DATATYPE vtRequestedDataType;
	unsigned short wReserved;
} ITEMDEF;

typedef struct __ITEMATTRIBUTES {
	char * szAccessPath;
	char * szItemID;
	bool bActive;
	long hClient;
	long hServer;
	unsigned long dwAccessRights;
	unsigned long dwBlobSize;
	char * pBlob;
	DATATYPE vtRequestedDataType;
	DATATYPE vtCanonicalDataType;
	EUTYPE dwEUType;
	DATAITEM * vEUInfo;
} ITEMATTRIBUTES;

typedef struct __ITEMRESULT {
	long hServer;
	DATATYPE vtCanonicalDataType;
	unsigned short wReserved;
	unsigned long dwAccessRights;
	unsigned long dwBlobSize;
	char * pBlob;
} ITEMRESULT;


