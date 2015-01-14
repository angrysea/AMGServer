#pragma once

#include <string>

class RegisteryNode;

extern char * KEY_LOCAL_MACHINE;

#define HKEY int
#define HKEY_LOCAL_MACHINE (( HKEY ) (unsigned long *)((long)0x80000002) )

class RegKey
{
public:
	bool Open(HKEY rootID, char * key);
	bool Open(RegKey & root, char * key);
	bool QueryStringValue(const std::string & name, char * value, long len);
	bool QueryINTValue(const std::string & name, int * value);
	
private:
	RegisteryNode * node;
};
