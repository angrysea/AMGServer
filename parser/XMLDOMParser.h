#pragma once

#include <string>
#include <stack>
#include <istream>
#include <sstream>
#include <exception>

#include "XMLReader.h"
#include "XMLDocument.h"
#include "../xmlconverter/XMLElementSerializer.h"
#include "../parser/DefaultHandler.h"
#include "../parser/XMLAttr.h"
#include "../xmlutils/NameMangler.h"

class PARSER_API XMLDOMParser : public DefaultHandler
{
public:
	XMLDOMParser() 
	{
	}

	void parse(const std::string & in) 
	{
		std::stringstream ss;
		ss.str(in.c_str());
		parse(&ss);
	}

	void parse(std::istream * is) 
	{
		try 
		{
			XMLReader parser;
			std::shared_ptr<DefaultHandler> defaultHandler(dynamic_cast<DefaultHandler *>(this));
			parser.setContentHandler(defaultHandler);
			parser.setErrorHandler(defaultHandler);
			parser.parse(is);
		} 
		catch (std::exception e) 
		{
			lastError = std::make_shared<std::exception>(e);
			std::cout << e.what();
			throw e;
		}
	}

	const std::shared_ptr<XMLDocument> & getDocument()
	{
		return document;
	}

	void processingInstruction(const std::string & target, const std::string & pi)
	{
	}

	void startDocument()
	{
		document = std::make_shared<XMLDocument>();
	}

	void endDocument()
	{
		document->appendChild(currentElement);
	}

	void startElement(const std::string & uri, const std::string & tag, const std::string & raw,
			Attributes & attrs)
	{
		bIgnoreCharacters = false;
		NameMangler nameMangler;
		try 
		{
			if (currentElement != nullptr) 
			{
				currentParent = currentElement;
				parentStack.push(currentElement);
			}
			currentElement = document->createElement(nameMangler.encode(tag));
			elementStack.push(currentElement);

			int len = attrs.getLength();
			std::string attrName;
			for (int i = 0; i < len; i++) 
			{
				attrName = attrs.getLocalName(i);
				attrName = nameMangler.encode(attrName);
				std::shared_ptr<XMLAttr> xmlattr = XMLNode::createAttribute(attrName);
				std::shared_ptr<XMLNode> text = document->createTextNode(attrs.getValue(i));
				xmlattr->appendChild(text);
				currentElement->setAttributeNode(xmlattr);
			}
			if (currentParent != nullptr)
				currentParent->appendChild(currentElement);
		} 
		catch (std::exception e) 
		{
			lastError = std::make_shared<std::exception>(e);
			std::string errormsg = "Error in Sax event startElement. Tag = " + tag;
			throw new std::exception(errormsg.c_str());
		}
	}

	void endElement(const std::string & uri, const std::string & tag, const std::string & raw)
	{
		bIgnoreCharacters = true;

		try 
		{
			if (parentStack.size() > 0)
			{
				parentStack.pop();
				if (parentStack.size()>0) 
				{
					currentParent = parentStack.top();
				}
			}

			if (elementStack.size()>0) 
			{
				elementStack.pop();
				if (elementStack.size()>0) 
				{
					currentElement = elementStack.top();
				}
			}
		}
		catch (std::exception e) 
		{
			lastError = std::make_shared<std::exception>(e);
			std::string errormsg = "Error in Sax event endElement. Tag = " + tag;
			throw new std::exception(errormsg.c_str());
		}
	}

	void characters(char buf[], int offset, int len)
	{
		try 
		{
			if (bIgnoreCharacters == false && len > 0) 
			{
				currentElement->setNodeValue(std::string(buf, offset, len));
			}
		}
		catch (std::exception e) 
		{
			lastError = std::make_shared<std::exception>(e);
			throw new std::exception("Error in Sax event characters.");
		}
	}

	void ignorableWhitespace(char buf[], int offset, int len)
	{
	}

	void comment(const std::string & text) 
	{
	}

	void endCDATA() 
	{
	}

	void endParsedEntity(const std::string & name, bool included) 
	{
	}

	void startCDATA() 
	{
	}

	void startParsedEntity(const std::string & name) 
	{
	}

	void error(std::exception e)
	{
		std::cout << "   " << e.what();
		lastError = std::make_shared<std::exception>(e);
		throw e;
	}

	void warning(std::exception e) 
	{
		std::cout << "   " << e.what();
		lastError = std::make_shared<std::exception>(e);
		throw e;
	}

	std::shared_ptr<std::exception> & getLastError()
	{
		return lastError;
	}

	bool hasError() 
	{
		return lastError != nullptr;
	}

protected:
	std::shared_ptr<XMLElementSerializer> & createElement(const std::string & name, const std::string & schemaURI,
		std::shared_ptr<XMLElementSerializer> & parent)
	{
		return std::shared_ptr<XMLElementSerializer>(new XMLElementSerializer(name, parent));
	}

	std::shared_ptr<XMLElementSerializer> & createElement(const std::string & name, const std::string & schemaURI,
		std::shared_ptr<XMLElementSerializer> & parent, bool bIsElement) 
	{
		return std::shared_ptr<XMLElementSerializer>(new XMLElementSerializer(name, parent, bIsElement));
	}

	std::shared_ptr<std::exception> lastError;

private:
	
	std::shared_ptr<XMLDocument> document;
	std::shared_ptr<XMLNode> currentElement;
	std::shared_ptr<XMLNode> currentParent;
	std::stack<std::shared_ptr<XMLNode>> elementStack;
	std::stack<std::shared_ptr<XMLNode>> parentStack;
	bool bIgnoreCharacters = true;
};