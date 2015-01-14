#pragma once

#include <string>
#include <map>

#include "export.h"
#include "XmlBase.h"
#include "../xmlutils/NameMangler.h"


class XMLCONVERTER_API AttributeType : public XmlBase
{
public:
	AttributeType() 
	{
	}

	void generateCode(std::string & ret);
	void generateProperty(std::string & ret);

	const std::string getMemberDefault(const std::string & type);
	std::string convertType();
	static std::string convertType(const std::string & inType);
	static void insertType(const std::string & simpleType, const std::string & xmlType);
	static bool isString(const std::string & type);
	void generateWrite(std::string & ret);
	void generateRead(std::string & ret);
	void generateCacheWrite(std::string & ret);
	void generateCacheRead(std::string & ret);

protected:
	static std::map<std::string, std::string> dataTypes;
	static const std::map<std::string, std::string> cacheTypes;
	static const std::map<std::string, std::string> writeMethods;
	static const std::map<std::string, std::string> readMethods;
};
