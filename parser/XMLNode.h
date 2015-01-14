#pragma once

#include <string>
#include <vector>
#include <memory>

#include "export.h"

enum PARSER_API NodeType
{
	UNKNOWN_NODE,
	DOCUMENT_NODE, 
	ELEMENT_NODE, 
	PROCESSING_INSTRUCTION_NODE, 
	CDATA_SECTION_NODE, 
	TEXT_NODE, 
	ENTITY_REFERENCE_NODE
};

class PARSER_API XMLAttr;

class PARSER_API XMLNode
{
public:
	XMLNode() = default;

	XMLNode(const std::string & name, NodeType type) :
		type{ type },
		name{ name }
	{
	}

	XMLNode(NodeType type) :
		type{ type }
	{
	}

	NodeType getNodeType() const 
	{
		return type;
	}

	void setNodeType(NodeType newValue) 
	{
		type = newValue;
	}

	const std::string & getNodeName() const
	{
		return name;
	}

	const std::string & getNodeValue() const
	{
		return value;
	}

	void setNodeValue(const std::string & newValue) 
	{
		value = newValue;
	}

	virtual void appendChild(std::shared_ptr<XMLNode> child)
	{
		children.push_back(child);
	}

	void getElementsByTagName(const std::string & tag, std::vector<std::shared_ptr<XMLNode>> byName) const
	{
		for(auto & node : children)
		{
			if (node->getNodeName()==tag)
				byName.push_back(node);
		}
	}

	const std::vector<std::shared_ptr<XMLNode>> & getChildNodes() const
	{
		return children;
	}

	std::shared_ptr<XMLNode> & getFirstChild() 
	{
		return children[0];
	}

	void setAttributeNode(std::shared_ptr<XMLAttr> & attr) 
	{
		attributes.push_back(attr);
	}

	std::vector<std::shared_ptr<XMLAttr>> & getAttributes()
	{
		return attributes;
	}

	void setAttribute(const std::string & Name, const std::string & value);
	const std::string & getAttribute(const std::string & name);
	std::shared_ptr<XMLNode> & createProcessingInstruction(const std::string & name, const std::string & value);
	std::shared_ptr<XMLNode> & createElement(const std::string & name);
	std::shared_ptr<XMLNode> & createTextNode(const std::string & text);
	static std::shared_ptr<XMLAttr> & createAttribute(const std::string & name);

private:
	NodeType type = UNKNOWN_NODE;
	std::string name;
	std::string value;
	std::vector<std::shared_ptr<XMLAttr>> attributes;
	std::vector<std::shared_ptr<XMLNode>> children;
};
