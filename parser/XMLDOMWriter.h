#pragma once

#include <string>
#include <memory>

#include "export.h"
#include "XMLNode.h"
#include "XMLDocument.h"

class PARSER_API XMLDOMWriter
{
public:

	XMLDOMWriter(bool canonical) :
		canonical { canonical }
	{
	}

	const std::string & writeXML(std::shared_ptr<XMLDocument> & document)
	{
		std::shared_ptr<XMLNode> docNode = document;
		write(docNode, false);
		return xmlBuffer;
	}

	void write(const std::shared_ptr<XMLNode> & node, bool bRootNode);
	const std::string & normalize(const std::string & s) const;
	std::shared_ptr<std::shared_ptr<XMLAttr>> sortAttributes(std::vector<std::shared_ptr<XMLAttr>> & attrs) const;

private:
	mutable std::string xmlBuffer;
	bool canonical;
};
