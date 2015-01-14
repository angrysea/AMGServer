#pragma once

#include <string>
#include <memory>

#include "export.h"
#include "XsdSchemaLoader.h"

class XMLCONVERTER_API XmlSchemaLoader
{
public:
	static bool bObjectAccessors;
	static std::string XSDSCHEMA;

	XmlSchemaLoader() = default; 
	void xmlLoad(XsdSchemaLoader & loader, std::istream * is);
	void generateClasses(const std::string & filePath,
		const std::string & schemaFileName, bool cacheable);

};
