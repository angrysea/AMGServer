#pragma once

#include "export.h"
#include "XmlBase.h"

class XMLCONVERTER_API ElementData : public XmlBase
{
public:

	ElementData() : XmlBase()
	{
	}

	ElementData(const std::string & e) : XmlBase(),
		elementType{ e },
		elementTagType{ e }
	{
	}

	ElementData(const std::string & name, const std::string & tag) : XmlBase(), 
		elementType{ name },
		elementTagType{ tag }
	{
	}

	void setElement(const std::string & e) 
	{
		elementType = e;
	}

	const std::string & getElementType() const 
	{
		return elementType;
	}

	void setElementTagType(const std::string & e) 
	{
		elementTagType = e;
	}

	const std::string & getElementTypeTag() const
	{
		return elementTagType;
	}

private:
	std::string elementType;
	std::string elementTagType;
};
