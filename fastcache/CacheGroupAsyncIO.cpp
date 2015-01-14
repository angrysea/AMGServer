#include "CacheGroup.h"

long CacheGroup::Cancel( unsigned long dwTransactionID)
{
	return RET_NOTIMPL;
}

long CacheGroup::Refresh( unsigned long dwConnection, DATASOURCE dwSource, unsigned long * pTransactionID)
{
	return RET_NOTIMPL;
}

long CacheGroup::Write( unsigned long dwConnection, unsigned long dwNumItems,  unsigned long * phServer, Item * pItemValues,  unsigned long * pTransactionID, long ** ppErrors)
{
	return RET_NOTIMPL;
}

long CacheGroup::Read(unsigned long dwConnection, DATASOURCE dwSource,unsigned long dwNumItems, unsigned long * phServer,unsigned long * pTransactionID, long ** ppErrors)
{
	return RET_NOTIMPL;
}
