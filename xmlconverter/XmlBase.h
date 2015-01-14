#pragma once
#include <string>
#include <map>
#include <memory>

#include "export.h"

class XMLCONVERTER_API XmlBase
{
public:
	XmlBase() 
	{
		bMaxOne = true;
		bOptional = false;
	}

	virtual ~XmlBase() = default;

	void setProperty(const std::string & key, const std::string & value) 
	{
		properties.insert(std::make_pair(key, value));
	}

	const std::string & getProperty(const std::string & key) const
	{
		auto & it = properties.find(key);
		if (it != properties.end())
			return (*it).second;
		return empty_str;
	}

	void putAll(std::shared_ptr<XmlBase> b) 
	{
		properties.insert(b->properties.begin(), b->properties.end());
	}

	bool getMaxOne() const
	{
		return bMaxOne;
	}

	void setMaxOne(bool newValue) 
	{
		bMaxOne = newValue;
	}

	void setOptional(bool newValue) 
	{
		bOptional = newValue;
	}

	bool getOptional() const
	{
		return bOptional;
	}

	int getMaxOccurs() const
	{
		return maxOccurs;
	}

	void setMaxOccurs(int newValue) 
	{
		maxOccurs = newValue;
	}

protected:
	std::map<std::string, std::string> properties;

private:
	bool bMaxOne;
	bool bOptional;
	int maxOccurs;
	static std::string empty_str;
};