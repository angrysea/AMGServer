#pragma once

#include <string>
#include "DataArray.h"

class CacheServer;

class ValueObject 
{
public:
	virtual DATAARRAY * write() = 0;
	virtual void read(DATAARRAY * in) = 0;
};
