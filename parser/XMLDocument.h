#pragma once

#include <memory>

#include "export.h"
#include "XMLNode.h"

class PARSER_API XMLDocument : public virtual XMLNode
{
public:
	
	XMLDocument() 
	{
		setNodeType(NodeType::DOCUMENT_NODE);
	}

	void appendChild(std::shared_ptr<XMLNode> & child) 
	{
		if (child->getNodeType() == NodeType::ELEMENT_NODE)
			docElement = child;
		else
			appendChild(child);
	}

	std::shared_ptr<XMLNode> & getDocumentElement()
	{
		return docElement;
	}

private:
	std::shared_ptr<XMLNode> docElement;
};
