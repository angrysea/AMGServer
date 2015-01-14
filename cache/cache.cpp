// cache.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CacheWorker.h"

const std::string CacheWorker::PUT = "/cache/put";
const std::string CacheWorker::REMOVE = "/cache/remove";
const std::string CacheWorker::SETLOCKED = "/cache/setLocked";
