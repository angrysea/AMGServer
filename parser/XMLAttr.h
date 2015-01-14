#pragma once

#include <string>
#include <memory>

#include "export.h"
#include "XMLNode.h"


class PARSER_API XMLAttr
{
public:
	
	XMLAttr(const std::string & name) :
		name{ name }
	{
	}

	const std::string & getNodeName() const 
	{
		return name;
	}

	const std::string & getNodeValue() const
	{
		return value->getNodeValue();
	}

	void appendChild(std::shared_ptr<XMLNode> & newValue)
	{
		value = newValue;
	}

private:
	std::string name;
	std::shared_ptr<XMLNode> value;
};
