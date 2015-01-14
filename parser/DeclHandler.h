#pragma once

#include <string>
#include <exception>

#include "export.h"

class PARSER_API DeclHandler
{
public:
	virtual void elementDecl(const std::string & name, const std::string & model) = 0;
	virtual void attributeDecl(const std::string & eName, const std::string & aName, const std::string & type,
		const std::string & valueDefault, const std::string & value);
	virtual void internalEntityDecl(const std::string & name, const std::string & value);
	virtual void externalEntityDecl(const std::string & name, const std::string & publicId,
			const std::string & systemId);
};
