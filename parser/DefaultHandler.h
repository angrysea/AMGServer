#pragma once

#include <string>
#include <istream>
#include <exception>

#include "export.h"
#include "Attributes.h"
#include "../entityobjects/DataArray.h"

class PARSER_API DefaultHandler
{
public:
	virtual std::istream * resolveEntity(const std::string & publicId, const std::string & systemId)
	{
		return nullptr;
	}

	virtual void notationDecl(const std::string & name, const std::string & publicId, const std::string & systemId)
	{
	}

	virtual void unparsedEntityDecl(const std::string & name, const std::string & publicId,
			const std::string & systemId, const std::string & notationName)
	{
	}

	virtual void startDocument()
	{
	}

	virtual void endDocument()
	{
	}

	virtual void startPrefixMapping(const std::string & prefix, const std::string & uri)
	{
	}

	virtual void endPrefixMapping(const std::string & prefix)
	{
	}

	virtual void startElement(const std::string & uri, const std::string & localName, const std::string & qName,
			Attributes & attributes)
	{
	}

	virtual void endElement(const std::string & uri, const std::string & localName, const std::string & qName)
	{
	}

	virtual void characters(char ch[], int start, int length)
	{
	}

	virtual void ignorableWhitespace(char ch[], int start, int length)
	{
	}

	virtual void processingInstruction(const std::string & target, const std::string & data)
	{
	}

	virtual void skippedEntity(const std::string & name)
	{
	}

	virtual void warning(std::exception & e)
	{
	}

	virtual void error(std::exception & e)
	{
	}

	virtual void fatalError(std::exception & e)
	{
		throw e;
	}

	virtual DATAARRAY * getObject()
	{
		return nullptr;
	}
};
