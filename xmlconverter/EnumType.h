#pragma once

#include <string>
#include <vector>
#include <memory>
#include <exception>

#include "export.h"
#include "XmlBase.h";
#include "../xmlutils/NameMangler.h"

class XMLCONVERTER_API EnumType : public XmlBase
{
public:
	EnumType() : XmlBase()
	{
	}

	void putEnumvalues(const std::string & value) 
	{
		try 
		{
			if (!value.empty())
				enumvalues.push_back(value);
		} catch (std::exception exception) {
		}
	}

	auto getEnums() -> std::vector<std::string>::iterator
	{
		return enumvalues.begin();
	}

	const std::string & generateCode() const;

	const std::string & getName() const
	{
		return getProperty("name");
	}

	static void insertType(const std::string & simpleType, const std::string & xmlType) 
	{
		try 
		{
			dataTypes.insert(std::make_pair(simpleType, xmlType));
		} 
		catch (std::exception e) 
		{
			std::cout << e.what() << std::endl;
		}
	}
private:
	static std::map<std::string, std::string> dataTypes;
	std::vector<std::string> enumvalues;
	mutable std::string ret;
};
