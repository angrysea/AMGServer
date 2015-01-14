#include "XMLAttr.h"
#include "XMLNode.h"

void XMLNode::setAttribute(const std::string & Name, const std::string & value)
{
	std::shared_ptr<XMLAttr> xmlattr(new XMLAttr{ name });
	std::shared_ptr<XMLNode> text = createTextNode(value);
	xmlattr->appendChild(text);
	setAttributeNode(xmlattr);
}

const std::string & XMLNode::getAttribute(const std::string & name) 
{
	for(auto & attr : attributes) 
	{
		if (attr->getNodeName()==name)
			return attr->getNodeValue();
	}
	return nullptr;
}

std::shared_ptr<XMLNode> & XMLNode::createProcessingInstruction(const std::string & name, const std::string & value) 
{
	std::shared_ptr<XMLNode> node(new XMLNode(name, PROCESSING_INSTRUCTION_NODE));
	node->setNodeValue(value);
	return node;
}

std::shared_ptr<XMLNode> & XMLNode::createElement(const std::string & name)
{
	return std::shared_ptr<XMLNode>(new XMLNode(name, ELEMENT_NODE));
}

std::shared_ptr<XMLNode> & XMLNode::createTextNode(const std::string & text) {
	std::shared_ptr<XMLNode> node(new XMLNode(TEXT_NODE));
	node->setNodeValue(text);
	return node;
}

std::shared_ptr<XMLAttr> & XMLNode::createAttribute(const std::string & name) 
{
	return std::shared_ptr<XMLAttr>(new XMLAttr(name));
}

