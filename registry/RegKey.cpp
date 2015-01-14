#include "RegKey.h"
#include "Registery.h"
#include "RegisteryNode.h"

char * KEY_LOCAL_MACHINE = "HKEY_LOCAL_MACHINE";


bool RegKey::Open(HKEY rootID, char * key)
{
	bool ret = false;
	RegisteryNode * root = Registery::getRootNode();

	if(rootID==0 || rootID==HKEY_LOCAL_MACHINE)
	{
		root = root->findNode(KEY_LOCAL_MACHINE);
		node = root->getNode(key);
		if(node!=0)
			ret=true;
	}
	return ret;
}

bool RegKey::Open(RegKey & root, char * key)
{
	bool ret = false;
	node = root.node->getNode(key);
	if(node!=0)
		ret=true;
	return ret;
}

bool RegKey::QueryStringValue(const std::string & name, char * value, long len)
{
	bool ret = false;
	RegisteryNode * attribute = node->findNode(name);
	if(attribute!=0 && attribute->getValueType()=="TEXT")
	{
		ret=true;
		memset(value, 0x00, len);
		memcpy(value, attribute->getValue().c_str(), len);
	}
	return ret;
}

bool RegKey::QueryINTValue(const std::string & name, int * value)
{
	bool ret = false;
	RegisteryNode * attribute = node->findNode(name);
	if(attribute!=0 && attribute->getValueType()=="INT")
	{
		*value=atoi(attribute->getValue().c_str());
	}
	return ret;
}

