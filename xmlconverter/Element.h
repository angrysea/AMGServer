#pragma once

#include "export.h"
#include "XmlBase.h"

class XMLCONVERTER_API Element : public XmlBase
{
public:
	Element() : XmlBase()
	{
	}

	Element(const std::string & e) : XmlBase(),
		elementType{ e }
	{
	}

	void setElement(const std::string & e) 
	{
		elementType = e;
	}

	std::string & getElementType() 
	{
		return elementType;
	}

private:
	std::string elementType;
};