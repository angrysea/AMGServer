#pragma once

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <memory>
#include <ostream>

#include "export.h"
#include "LoaderBase.h"
#include "XMLElementSerializer.h"
#include "../parser/DefaultHandler.h"

class XMLCONVERTER_API XsdSchemaLoader : public DefaultHandler, public LoaderBase
{
public:
	static std::string S_NAME;
	static std::string S_TYPE;
	static std::string S_ATTRIBUTEGROUP;
	static std::string S_GROUP;
	static std::string S_BASE;
	static std::string S_SIMPLETYPE;
	static std::string S_RESTRICTION;
	static std::string S_COMPLEXTYPE;
	static std::string S_ELEMENT;
	static std::string S_MINOCCURS;
	static std::string S_MAXOCCURS;
	static std::string S_REF;
	static std::string S_ATTRIBUTE;
	static std::string S_SEQUENCE;
	static std::string S_BASETYPE;
	static std::string S_CHOICE;

	XsdSchemaLoader():
		currentElement{ nullptr },
		currentParent{ nullptr }
	{

	}
	virtual void xmlLoad(std::istream * is);
	virtual void xmlLoad(const std::string & data);
	std::map<const std::string, std::shared_ptr<ElementType>> & getElementTypes();
	void processTree();
	void enumerateChildren(std::shared_ptr<XMLElementSerializer> & e, std::shared_ptr<ElementType> & elementType,
		int minOccurs, int maxOccurs, const std::shared_ptr<XMLElementSerializer> & inner);
	virtual void startElement(const std::string & uri, const std::string & tag, const std::string & raw,
		Attributes & attrs);
	virtual void endElement(const std::string & uri, const std::string & tag, const std::string & raw);
	virtual void characters(char buf[], int offset, int len);
	virtual void ignorableWhitespace(char ac[], int i, int j);
	virtual void comment(const std::string & s);
	virtual void endCDATA();
	virtual void endParsedEntity(const std::string & s, bool flag);
	virtual void startCDATA();
	virtual void startPrefixMapping(const std::string & s, const std::string & s1);
	virtual void endPrefixMapping(const std::string & s);
	virtual void startParsedEntity(const std::string & s);
	virtual void error(std::exception & e) const;
	virtual void warning(std::exception & e) const;

private:
	
	const std::string & getValueFromString(const std::string & str, const std::string & name);
	virtual std::shared_ptr<ElementType> getElementType(const std::string & type);
	std::shared_ptr<XMLElementSerializer> getSimpleType(const std::string & type);
	//int start();
	//void end();
	//const std::string & next(bool externalizable);
	void dump(std::ostream & stream, bool externalizable);

private:
	std::map<const std::string, std::shared_ptr<XMLElementSerializer>> attributeGroups;
	std::map<const std::string, std::shared_ptr<XMLElementSerializer>> groups;
	std::map<const std::string, std::shared_ptr<XMLElementSerializer>> simpleTypes;
	std::map<const std::string, std::shared_ptr<XMLElementSerializer>> complexTypes;
	std::map<const std::string, std::shared_ptr<XMLElementSerializer>> elements;
	std::map<const std::string, std::shared_ptr<ElementType>> elementTypes;
	std::shared_ptr<XMLElementSerializer> currentElement;
	std::shared_ptr<XMLElementSerializer> currentParent;
	std::stack<std::shared_ptr<XMLElementSerializer>> elementStack;
	std::stack<std::shared_ptr<XMLElementSerializer>> parentStack;
	bool bIgnoreCharacters;
};
