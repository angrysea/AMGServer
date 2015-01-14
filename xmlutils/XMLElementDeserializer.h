#pragma once
#include <string>

#include "AliasProcessor.h"

#include "../parser/XMLDocument.h"

#include "../entityobjects/DataArray.h"

#include "../parser/XMLAttr.h"
#include "../parser/XMLDOMWriter.h"
#include "../parser/XMLDocument.h"
#include "../parser/XMLNode.h"

class XMLElementDeserializer: public AliasProcessor 
{
public:
	const std::string & get(DATAARRAY * o, const std::string & pi, bool bXMLPI, const std::string & agentPIAttribs);
	XMLNode unwindProperties(DATAARRAY * o, const std::string & name, const std::string & parentType);
	const std::string & getPrimitive(const std::string & className, DATAARRAY * o, int i);
	XMLNode createElement(const std::string & className, const std::string & fieldName);
	XMLAttr createAttribute(const std::string & className, const std::string & fieldName);

private:
	XMLDocument domDocument;
	XMLNode root;
};
