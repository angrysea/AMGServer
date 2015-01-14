
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <exception>

#include "export.h"
#include "XmlBase.h"
#include "AttributeType.h"
#include "ElementData.h"
#include "../xmlutils/NameMangler.h"

class XMLCONVERTER_API ElementType : public XmlBase
{
public:
	ElementType();
	void putAttribute(std::shared_ptr<AttributeType> & attrib);
	void ElementType::putElement(std::shared_ptr<ElementData> & element);
	std::vector<std::shared_ptr<ElementData>>::iterator getElements();
	std::shared_ptr<ElementData> getElementByType(const std::string & type);
	const std::string generateCode(bool cacheable);
	void generateWrite(std::string & ret) const;
	void generateRead(std::string & ret) const;
	void generateCacheWrite(std::string & ret) const;
	void generateCacheRead(std::string & ret) const;
	bool isJavaType(const std::string & className);
	void getInitInfo(const std::string & className, std::shared_ptr<ElementData> & e) const;
	const std::string getName() const;
	static const std::string convertType(const std::string & inType);
	static const std::string convertToNativeType(const std::string & inType);
	static void insertType(const std::string & simpleType, const std::string & xmlType);

protected:
	static std::map<std::string, std::string > dataTypes;
	static std::map<std::string, std::string > javaTypes;
	static std::map<std::string, std::string > cacheTypes;
	static std::map<std::string, std::string > dataItemTypes;
	static std::map<std::string, std::string > writeMethods;
	static std::map<std::string, std::string > readMethods;
	static std::map<std::string, std::string > nonNativeTypes;


private:
	std::vector<std::shared_ptr<AttributeType>> attributes;
	std::vector<std::shared_ptr<ElementData>> elements;
	mutable std::string ret;
};