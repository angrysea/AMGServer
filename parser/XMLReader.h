#pragma once

#include <map>
#include <memory>

#include "export.h"
#include "BaseParser.h"
#include "DefaultHandler.h"
#include "../entityobjects/DataArray.h"

class PARSER_API XMLReader : public BaseParser
{
public:
	XMLReader() = default;

	void setContentHandler(DefaultHandler * newHandler) 
	{
		handler = newHandler;
	}

	void setErrorHandler(DefaultHandler * newErrorHandler) 
	{
		errorHandler = newErrorHandler;
	}

	DATAARRAY * parse(std::istream * in);
	bool isPI(const std::string & chars);

protected:
	void parseElement();
	void parseName(const std::string & name);

	static void normalize(std::string & chars, int start, int len);

private:
	std::string normalize(std::string & value);

protected:
	Attributes attrs;
	std::map<std::string, std::string> namespaces;
	std::string tname;
	std::string turi;
	DATAARRAY * obj;
	DefaultHandler * handler;
	DefaultHandler * errorHandler;
};
