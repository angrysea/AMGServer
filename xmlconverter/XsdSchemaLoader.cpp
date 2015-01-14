#include <istream>
#include <sstream>
#include <limits>

#include "XsdSchemaLoader.h"
#include "EqualsIgnoreCase.h"
#include "../parser/XMLReader.h"

std::string XsdSchemaLoader::S_NAME = "name";
std::string XsdSchemaLoader::S_TYPE = "type";
std::string XsdSchemaLoader::S_ATTRIBUTEGROUP = "attributeGroup";
std::string XsdSchemaLoader::S_GROUP = "group";
std::string XsdSchemaLoader::S_BASE = "base";
std::string XsdSchemaLoader::S_SIMPLETYPE = "simpleType";
std::string XsdSchemaLoader::S_RESTRICTION = "restriction";
std::string XsdSchemaLoader::S_COMPLEXTYPE = "complexType";
std::string XsdSchemaLoader::S_ELEMENT = "element";
std::string XsdSchemaLoader::S_MINOCCURS = "minOccurs";
std::string XsdSchemaLoader::S_MAXOCCURS = "maxOccurs";
std::string XsdSchemaLoader::S_REF = "ref";
std::string XsdSchemaLoader::S_ATTRIBUTE = "attribute";
std::string XsdSchemaLoader::S_SEQUENCE = "sequence";
std::string XsdSchemaLoader::S_BASETYPE = "baseType";
std::string XsdSchemaLoader::S_CHOICE = "choice";

void XsdSchemaLoader::xmlLoad(std::istream * is)
{
	XMLReader parser;
	parser.setContentHandler(this);
	parser.setErrorHandler(this);
	parser.parse(is);
	processTree();
}

void XsdSchemaLoader::xmlLoad(const std::string & data)
{
	XMLReader parser;
	parser.setContentHandler(this);
	parser.setErrorHandler(this);
	std::istringstream in;
	in.str(data);
	parser.parse(&in);
	processTree();
}

std::map<const std::string, std::shared_ptr<ElementType>> & XsdSchemaLoader::getElementTypes()
{
	return elementTypes;
}

void XsdSchemaLoader::processTree() 
{
	currentElement->update(currentElement, attributeGroups, "attributeGroup");
	currentElement->update(currentElement, groups, "group");
	currentElement->update(currentElement, simpleTypes, "simpleType", false);
	for(auto & p : simpleTypes)
	{
		auto & e = p.second;
		std::string name = e->getProperty("name");
		if (!name.empty()) 
		{
			AttributeType::insertType(name, e->getProperty("base"));
		}
	}
	currentElement->update(currentElement, complexTypes, "complexType");
	currentElement->update(currentElement, elements, "element");
	for (auto & p :complexTypes)
	{
		auto & e = p.second;
		std::shared_ptr<ElementType> elementType(new ElementType);
		elementType->putAll(e);
		enumerateChildren(e, elementType, -1, -1, nullptr);
		const std::string & name = e->getProperty("name");
		elementTypes.insert(std::make_pair(name, elementType));
	}

	for (auto & p : elements)
	{
		auto & e = p.second;
		const std::string & name = e->getProperty("name");
		std::string type = e->getProperty("type");
		if (type.empty()) 
		{
			std::shared_ptr<ElementType> elementType (new ElementType);
			elementType->putAll(e);
			enumerateChildren(e, elementType, -1, -1, nullptr);
			elementTypes.insert(std::make_pair(name, elementType));
		}
	}
}

void XsdSchemaLoader::enumerateChildren(std::shared_ptr<XMLElementSerializer> & e,
	std::shared_ptr<ElementType> & elementType,
	int minOccurs, int maxOccurs, 
	const std::shared_ptr<XMLElementSerializer> & inner) 
{
	for (auto & child : e->children())
	{
		std::string type = child->getProperty("type");
		std::string childname = child->getName();
		if (equalsIgnoreCase(childname, "complextype"))
		{
			elementType->putAll(child);
			enumerateChildren(child, elementType, -1, -1, inner);
		}
		else if (equalsIgnoreCase(childname, "element"))
		{
			bool enumChildren = true;
			if (type.empty()) 
			{
				std::string ref = child->getProperty("ref");
				if (!ref.empty()) 
				{
					auto & it = elements.find(ref);
					if (it != elements.end()) 
					{
						auto & refchild = (*it).second;
						child->setProperty("name", refchild->getProperty("name"));
						type = refchild->getProperty("type");
						if (type.empty())
							type = ref;
					}
					enumChildren = false;
				}
			}
				
			if (enumChildren) 
			{
				enumerateChildren(child, elementType, -1, -1, inner);
				if (type.empty())
					type = child->getProperty("type");
			}

			if (!type.empty()) 
			{
				int colon = type.find_first_of(':');
				if (colon != std::string::npos)
					type = ElementType::convertType(type.substr(colon + 1));
				std::shared_ptr<ElementData> 
					element(new ElementData(child->getProperty("name"), type));
				element->putAll(child);
				
				if (minOccurs > -1) 
				{
					std::string property = element->getProperty("minOccurs");
					if (property.empty())
						element->setProperty("minOccurs", std::to_string(minOccurs));
				}

				if (maxOccurs > 0) 
				{
					std::string property = element->getProperty("maxOccurs");
					if (property.empty())
						element->setProperty("maxOccurs", std::to_string(maxOccurs));
				}
				elementType->putElement(element);
			}
		}
		else if (equalsIgnoreCase(childname, "attribute"))
		{
			if (!type.empty()) 
			{
				std::shared_ptr<AttributeType> attribute(new AttributeType());
				attribute->putAll(child);
				int loc = type.find_first_of(':')+1;
				if (loc > 0)
					attribute->setProperty("type", type.substr(loc));
				else
					attribute->setProperty("type", type);
				elementType->putAttribute(attribute);
			}
		}
		else if (equalsIgnoreCase(childname, "simpletype"))
		{
			std::string name = child->getProperty("name");
			if (name.empty())
				elementType->putAll(child);
			enumerateChildren(child, elementType, minOccurs, maxOccurs, e);
		}
		else if (equalsIgnoreCase(childname, "attributegroup"))
		{
			std::string ref = child->getProperty("ref");
			if (!ref.empty()) 
			{
				auto & it = attributeGroups.find(ref);
				if (it!=attributeGroups.end())
				{
					auto & attributeGroup = (*it).second;
					enumerateChildren(attributeGroup, elementType, -1, -1, inner);
				}
			}
		}
		else if (equalsIgnoreCase(childname, "group"))
		{
			std::string ref = child->getProperty("ref");
			if (!ref.empty()) 
			{
				auto & it = groups.find(ref);
				auto & group = (*it).second;
				enumerateChildren(group, elementType, -1, -1, inner);
			}
		}
		else if (equalsIgnoreCase(childname, "sequence"))
		{
			int min = -1;
			int max = 1;
			std::string property = child->getProperty("minOccurs");
			if (!property.empty())
				min = std::atoi(property.c_str());
			property = child->getProperty("maxOccurs");
			if (!property.empty())
				if (property=="unbounded")
					max = std::numeric_limits<int>::max();
				else
					max = std::atoi(property.c_str());
			enumerateChildren(child, elementType, min, max, inner);
		}
		else if (equalsIgnoreCase(childname, "restriction"))
		{
			const std::string & type = child->getProperty("base");
			if (!type.empty() && inner != nullptr)
				inner->setProperty("type", type);
		}
		else if (equalsIgnoreCase(childname, "choice"))
		{
			int min = minOccurs;
			int max = maxOccurs;
			if (min < 0)
				min = 0;
			if (max < 0)
				max = 1;
			enumerateChildren(child, elementType, min, max, inner);
		}
		else 
		{
			enumerateChildren(child, elementType, -1, -1, inner);
		}
	}
}


void XsdSchemaLoader::startElement(const std::string & uri, const std::string & tag, 
	const std::string & raw, Attributes & attrs)
{
	bIgnoreCharacters = false;
	if (currentElement != nullptr) 
	{
		currentParent = currentElement;
		parentStack.push(currentElement);
	}
	currentElement = std::make_shared<XMLElementSerializer>(tag);
	if (currentParent != nullptr)
		currentParent->addChild(currentElement);

	int len = attrs.getLength();
	for (int i = 0; i < len; i++)
		currentElement->setProperty(attrs.getLocalName(i), attrs.getValue(i));

	elementStack.push(currentElement);
}

void XsdSchemaLoader::endElement(const std::string & uri, const std::string & tag, const std::string & raw)
{
	bIgnoreCharacters = true;
	if (!parentStack.empty()) 
	{
		parentStack.pop();
		if (!parentStack.empty())
			currentParent = parentStack.top();
	}

	if (!elementStack.empty()) 
	{
		elementStack.pop();
		if (!elementStack.empty())
			currentElement = elementStack.top();
	}
}

void XsdSchemaLoader::characters(char buf[], int offset, int len)
{
	if (!bIgnoreCharacters && len > 0 && currentElement!=nullptr)
		currentElement->setData(buf, offset, len);
}

void XsdSchemaLoader::ignorableWhitespace(char ac[], int i, int j)
{
}

void XsdSchemaLoader::comment(const std::string & s) {
}

void XsdSchemaLoader::endCDATA() {
}

void XsdSchemaLoader::endParsedEntity(const std::string & s, bool flag)
{
}

void XsdSchemaLoader::startCDATA() {
}

void XsdSchemaLoader::startPrefixMapping(const std::string & s, const std::string & s1) 
{
}

void XsdSchemaLoader::endPrefixMapping(const std::string & s)
{
}

void XsdSchemaLoader::startParsedEntity(const std::string & s) 
{
}

void XsdSchemaLoader::error(std::exception & ex) const
{
	std::cout << "   " << ex.what() << std::endl;
	throw ex;
}

void XsdSchemaLoader::warning(std::exception & e) const
{
	std::cout << "   " << e.what() << std::endl;
	throw e;
}

const std::string & XsdSchemaLoader::getValueFromString(const std::string & str, const std::string & name) 
{
	std::string value;
	int start = str.find(name);
	if (start != std::string::npos) 
	{
		start++;
		int end = str.find_first_of('\'', start+1);
		if (end!=-1)
			value = str.substr(start, (end-start)+1);
	}
	return value;
}

std::shared_ptr<ElementType> XsdSchemaLoader::getElementType(const std::string & type)
{
	auto & it = elementTypes.find(type);
	if (it != elementTypes.end())
		return (*it).second;
	return std::shared_ptr<ElementType>();
}

std::shared_ptr<XMLElementSerializer> XsdSchemaLoader::getSimpleType(const std::string & type) 
{
	auto & it = simpleTypes.find(type);
	if (it != simpleTypes.end())
		return (*it).second;
	return std::shared_ptr<XMLElementSerializer>();
}

void XsdSchemaLoader::dump(std::ostream & os, bool externalizable) 
{
	for(auto & p : elementTypes)
	{
		auto & e = p.second;
		if (e != nullptr) 
		{
			std::string classData = e->generateCode(externalizable);
			if (!classData.empty())
				os.write(classData.c_str(), classData.length());
		}
	}
}


/*
int XsdSchemaLoader::start()
{
itTypes = elementTypes.begin();
return elementTypes.size();
}

void XsdSchemaLoader::end()
{
itTypes = elementTypes.end();
}

const std::string & XsdSchemaLoader::next(bool externalizable)
{
std::string classData;
itTypes++;
if (itTypes != elementTypes.end())
{
auto & e = (*it).second;
if (e != nullptr)
{
classData = e->generateCode(externalizable);
if (!classData.empty())
return classData;
}
}
return classData;
}
*/

