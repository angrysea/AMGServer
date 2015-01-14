#include "ElementType.h"
#include "XmlSchemaLoader.h"
#include "EqualsIgnoreCase.h"

std::map<std::string, std::string> ElementType::dataTypes
{
	{ "bool", "Boolean" },
	{ "string", "String" },
	{ "enumeration", "String" },
	{ "string.ansi", "String" },
	{ "fixed.14.4", "Float" },
	{ "float", "Float" },
	{ "i4", "Integer" },
	{ "long", "Long" },
	{ "unsignedLong", "Long" },
	{ "short", "Short" },
	{ "unsignedShort", "Short" },
	{ "double", "Double" },
	{ "date", "Date" },
	{ "dateTime", "Date" },
	{ "dateTime->iso8601", "Date" },
	{ "dateTime->iso8601.tz", "Date" },
	{ "date->iso8601", "Date" },
	{ "time->iso8601", "Date" },
	{ "time->iso8601.tz", "Date" },
	{ "number", "String" },
	{ "int", "Integer" },
	{ "integer", "Integer" },
	{ "i1", "Byte" },
	{ "i2", "Short" },
	{ "i4", "Integer" },
	{ "i8", "Long" },
	{ "ui1", "Byte" },
	{ "ui2", "Short" },
	{ "ui4", "Integer" },
	{ "positiveInteger", "Integer" },
	{ "nonPositiveInteger", "Integer" },
	{ "negativeInteger", "Integer" },
	{ "nonNegativeInteger", "Integer" },
	{ "ui8", "Long" },
	{ "r4", "Float" },
	{ "r8", "Double" },
	{ "decimal", "Double" },
	{ "float.IEEE.754.32", "Float" },
	{ "float.IEEE.754.64", "Double" },
	{ "char", "Character" },
	{ "bin.hex", "String" },
	{ "uri", "String" },
	{ "uuid", "String" },
	{ "uriReference", "String" }
};

std::map<std::string, std::string> ElementType::javaTypes
{
	{ "Float", "float" },
	{ "Integer", "int" },
	{ "Long", "long" },
	{ "Short", "short" },
	{ "Double", "double" },
	{ "Boolean", "bool" },
	{ "Byte", "byte" },
	{ "Date", "long" },
	{ "Character", "char" }
};

std::map<std::string, std::string> ElementType::nonNativeTypes
{
	{ "float", "float" },
	{ "integer", "int" },
	{ "long", "long" },
	{ "short", "short" },
	{ "double", "double" },
	{ "boolean", "bool" },
	{ "byte", "byte" },
	{ "character", "char" },
	{ "string", "string" },
	{ "date", "Date" }
};

std::map<std::string, std::string> ElementType::cacheTypes
{
	{ "Boolean", "bool" },
	{ "String", "string" },
	{ "Float", "Float" },
	{ "Integer", "Int" },
	{ "Long", "Long" },
	{ "Short", "Short" },
	{ "Double", "Double" },
	{ "Date", "Date" },
	{ "Byte", "Byte" },
	{ "Character", "Byte" }
};

std::map<std::string, std::string> ElementType::dataItemTypes
{
	{ "Boolean", "DATAITEMBOOLEAN" },
	{ "string", "DATAITEMSTRING" },
	{ "Float", "DATAITEMFLOAT" },
	{ "Integer", "DATAITEMINT" },
	{ "Long", "DATAITEMLONG" },
	{ "Short", "DATAITEMSHORT" },
	{ "Double", "DATAITEMDOUBLE" },
	{ "Date", "DATAITEMDATE" },
	{ "Byte", "DATAITEMBYTE" },
	{ "Character", "DATAITEMBYTE" }
};

std::map<std::string, std::string> ElementType::writeMethods
{
	{ "Boolean", "out.writeBoolean(" },
	{ "string", "out.writeUTF(" },
	{ "Float", "out.writeFloat(" },
	{ "Integer", "out.writeInt(" },
	{ "Long", "out.writeLong(" },
	{ "Short", "out.writeShort(" },
	{ "Double", "out.writeDouble(" },
	{ "Date", "writeDate(out, " },
	{ "Byte", "out.writeChar(" },
	{ "Character", "out.writeChar(" }
};

std::map<std::string, std::string> ElementType::readMethods
{
	{ "Boolean", "in.readBoolean(" },
	{ "string", "in.readUTF(" },
	{ "Float", "in.readFloat(" },
	{ "Integer", "in.readInt(" },
	{ "Long", "in.readLong(" },
	{ "Short", "in.readShort(" },
	{ "Double", "in.readDouble(" },
	{ "Date", "readDate(in" },
	{ "Byte", "in.readChar(" },
	{ "Character", "in.readChar(" },
};

ElementType::ElementType() : XmlBase()
{
}

void ElementType::putAttribute(std::shared_ptr<AttributeType> & attrib)
{
	try
	{
		attributes.push_back(attrib);
	}
	catch (std::exception exception)
	{
	}
}

void ElementType::putElement(std::shared_ptr<ElementData> & element)
{
	try 
	{
		elements.push_back(element);
	}
	catch (std::exception exception) 
	{
	}
}

std::vector<std::shared_ptr<ElementData>>::iterator ElementType::getElements()
{
	return elements.begin();
}

std::shared_ptr<ElementData> ElementType::getElementByType(const std::string & type)
{
	std::shared_ptr<ElementData> ret;
	for (auto data : elements) 
	{
		if (data != nullptr && data->getElementType() == type)
		{
			ret = data;
			break;
		}
	}
	return ret;
}

const std::string ElementType::generateCode(bool cacheable)
{
	std::string content;
	std::string mixed;
	std::string model;
	std::string base;
	bool bText = false;
	bool bContent = false;
	bool bConstructor = true;

	try 
	{
		auto & it = properties.find("name");
		if (it != properties.end()) 
		{
			NameMangler nameMangler;
			std::string & name = (*it).second;
			name = nameMangler.encode(name);
			content = getProperty("content");
			mixed = getProperty("mixed");
			base = getProperty("base");
			base = nameMangler.encode(base);
			model = getProperty("model");
			if (!model.empty() && model == "open") 
			{
				bText = true;
				bContent = true;
			}
			else if (!content.empty()) 
			{
				if (content == "empty") 
				{
					bContent = false;
					bText = false;
				}
				else if (content == "eltOnly") 
				{
					bContent = false;
					bText = false;
				}
				else if (content == "textOnly") 
				{
					bContent = false;
					bText = true;
				}
				else if (content == "mixed") 
				{
					bContent = false;
					bText = true;
				}
			}
			else if (!mixed.empty() && mixed == "true") 
			{
				bContent = false;
				bText = true;
			}
			else 
			{
				bText = false;
			}
			
			int size = elements.size();
			if (size > 0)
				bConstructor = false;

			bool bAddIts = false;
			bool bAddDate = false;
			bool bHasArrays = false;
			bool bHasNonJava = false;
			bool bHasNonJavaArray = false;

			for (auto e : elements) 
			{
				int nMaxOccurs = 0;
				if (e != nullptr) 
				{
					const std::string & optional = e->getProperty("minOccurs");
					if (!optional.empty() && optional == "0")
					{
						e->setOptional(true);
					}

					const std::string & maxOccurs = e->getProperty("maxOccurs");
					if (!maxOccurs.empty()) 
					{
						if (maxOccurs == "?")
							e->setOptional(true);
						if (maxOccurs == "*" || maxOccurs == "unbounded")
						{
							e->setMaxOne(false);
							e->setMaxOccurs(nMaxOccurs);
							bAddIts = true;
							bHasArrays = true;
						}
						else 
						{
							nMaxOccurs = std::atoi(maxOccurs.c_str());
							if (nMaxOccurs == 1) 
							{
								e->setMaxOne(true);
							}
							else 
							{
								e->setMaxOne(false);
								e->setMaxOccurs(nMaxOccurs);
								bHasArrays = true;
							}
						}
					}
					else 
					{
						e->setMaxOne(false);
						e->setMaxOccurs(nMaxOccurs);
						bHasArrays = true;
					}

					const std::string & order = e->getProperty("order");
					if (!order.empty() && order == "one")
					{
						e->setMaxOne(true);
					}

					std::string type = e->getElementTypeTag();
					if (equalsIgnoreCase(type, "date"))
					{
						bAddDate = true;
					}
					else
					{
						if (nonNativeTypes.find(type) == nonNativeTypes.end())
						{
							bHasNonJava = true;
							if (nMaxOccurs != 1) 
							{
								bHasNonJavaArray = true;
							}
						}
					}
				}
			}

			for (auto a : attributes) 
			{
				if (a == nullptr)
					continue;
				std::string nativeType = a->convertType();
				if (!nativeType.empty())
				{
					if (equalsIgnoreCase(nativeType, "date"))
					{
						bAddDate = true;
					}
				}
			}

			ret.append("#include <string>\n");
			if (bAddDate)
			{
				ret.append("#include <functional>\n");
				ret.append("#include <chrono>\n");
			}

			if (bAddIts) 
			{
				ret.append("#include <vector>\n");
			}

			if (cacheable) 
			{
				ret.append("#include \"..\\entityobjects\\ValueObject.h\"\n");
				ret.append("#include \"..\\entityobjects\\DataArray.h\"\n");
				ret.append("#include \"..\\entityobjects\\DataItem.h\"\n");
			}

			for (auto e : elements)
			{
				std::string elementType = e->getElementTypeTag();
				int start = elementType.find(':');
				if (start != std::string::npos)
				{
					elementType = elementType.substr(start + 1);
				}
				if (convertToNativeType(elementType).empty())
				{
					ret.append("#include \"");
					ret.append(elementType);
					ret.append(".h\"\n");
				}
			}

			ret.append("\nusing namespace std;\n");

			ret.append("\nclass ");
			ret.append(name);
			if (cacheable)
			{
				ret.append(" : public ValueObject ");
			}

			if (!base.empty())
			{
				ret.append(", public ");
				ret.append(base);
			}

			ret.append("\n{\npublic:\n");

			if (bText) 
			{
				std::string type = convertType(getProperty("type"));
				if (!type.empty()) 
				{
					NameMangler nameMangler;
					type = nameMangler.encode(type);
					if (bConstructor) 
					{
						ret.append("\t");
						ret.append(name);
						ret.append("() \n\t{\n");
						ret.append("\t}\n");
						ret.append("\t");
						ret.append(name);
						ret.append("(");
						ret.append(type);
						ret.append(" _contentData) \n\t\t{\n");
						ret.append("\t\t\tthis._contentData=_contentData;\n\t\t}\n");
					}
					ret.append("\t");
					ret.append(type);
					ret.append(" getContentData() \n\t{\n\t\treturn _contentData;\n\t}\n");
					ret.append("\tvoid setContentData(");
					ret.append(type);
					ret.append(" newValue) \n\t{\n\t\t _contentData = newValue;\n\t}\n");
				}
				else 
				{
					if (bConstructor) 
					{
						ret.append("\t");
						ret.append(name);
						ret.append("() \n\t{\n");
						ret.append("\t}\n");
						ret.append("\t");
						ret.append(name);
						ret.append("(std::string  _contentData) \n\t{\n");
						ret.append("\t\t this._contentData=_contentData;\n\t}\n");
					}
					ret.append("\tstd::string  getContentData() \n\t{\n\t\treturn _contentData;\n\t}\n");
					ret.append("\tvoid setContentData(std::string  newValue) \n\t{\n\t\t_contentData = newValue;\n\t}\n");
				}
			}

			for (auto e : elements) 
			{
				if (e != nullptr) 
				{
					NameMangler nameMangler;
					std::string elementName = e->getElementType();
					elementName = nameMangler.encode(elementName);
					std::string elementType = e->getElementTypeTag();
					int start = elementType.find(':');
					if (start != std::string::npos)
					{
						elementType = elementType.substr(start + 1);
					}
					if (!elementName.empty()) 
					{
						if (!e->getMaxOne()) 
						{
							int maxOccurs = e->getMaxOccurs();
							ret.append("\tint get");
							ret.append(elementName);
							ret.append("Count() \n\t{\n\t\treturn _");
							ret.append(elementName);
							ret.append("Count;\n\t}\n\t");
							ret.append(elementType);
							ret.append("[] get");
							ret.append(elementName);
							ret.append("Array()\n\t{\n\t\treturn _");
							ret.append(elementName);
							ret.append(";\n\t}\n\tvoid set");
							ret.append(elementName);
							ret.append("Array(");
							ret.append(elementType);
							ret.append(" newValue[])\n\t{\n\t\t_");
							ret.append(elementName);
							ret.append("=newValue;\n\t}\n");
							ret.append("\tIterator get");
							ret.append(elementName);
							ret.append("Iterator()\n\t{\n\t\treturn new Iterator() {\n\t\t\tint cursor=0;\n\t\t\tint last=-1;\n");
							ret.append("\t\t\tbool hasNext()\n\t{\n\t\t\t\tif(_");
							ret.append(elementName);
							ret.append("==null) return false;\n\t\t\t\tif(last<0) {");
							ret.append("\n\t\t\t\t\tlast = _");
							ret.append(elementName);
							ret.append(".length;\n\t\t\t\t\twhile(--last>-1&&_");
							ret.append(elementName);
							ret.append("[last]==null);\n\t\t\t\t\tlast++;\n\t\t\t\t}");
							ret.append("\n\t\t\t\treturn cursor!=last;\n");
							ret.append("\t\t\t}\n\t\t\tObject next() \n\t{\n\t\t\t\ttry {\n\t\t\t\t\treturn _");
							ret.append(elementName);
							ret.append("[cursor++];\n\t\t\t\t} catch(IndexOutOfBoundsstd::exception e) {");
							ret.append("\n\t\t\t\t\tthrow new NoSuchElementException();\n\t\t\t\t}\n\t\t\t}\n");
							ret.append("\t\t\tvoid remove() \n\t{\n\t\t\t\ttry {\n\t\t\t\t\t_");
							ret.append(elementName);
							ret.append("[cursor++]=null;\n\t\t\t\t} catch(IndexOutOfBoundsstd::exception e) \n\t\t\t\t{");
							ret.append("\n\t\t\t\t\tthrow new NoSuchElementException();\n\t\t\t\t}\n\t\t\t}\n\t\t};\n\t}\n");
							
							const std::string & nativeType = convertToNativeType(elementType);
							if (nativeType.empty()
								|| XmlSchemaLoader::bObjectAccessors)
							{
								ret.append("\t");
								ret.append(elementType);
								ret.append(" get");
								ret.append(elementName);
								ret.append("(int idx) \n\t{ \n\t\treturn (");
								ret.append(elementType);
								ret.append(")_");
								ret.append(elementName);
								ret.append("[idx];\n\t}\n\tvoid set");
								ret.append(elementName);
								ret.append("(");
								ret.append(elementType);
								ret.append(" newValue) \n\t{\n\t\tif(_");
								ret.append(elementName);
								ret.append("!=null) \n\t\t{\n\t\t\tint __OPEN_A=-1;\n\t\t\tfor(int __I_A=0;__I_A<_");
								ret.append(elementName);
								ret.append(".length;__I_A++) \n\t\t\t{\n\t\t\t\tif(_");
								ret.append(elementName);
								ret.append("[__I_A]==null) \n\t\t\t{\n\t\t\t\t\t__OPEN_A=__I_A;\n\t\t\t\t\tbreak;\n\t\t\t\t}\n");
								ret.append("\t\t\t}\n\t\t\tif(__OPEN_A<0) \n\t\t\t{\n");
								if (maxOccurs > 0) 
								{
									ret.append("\t\t\t\tthrow new IndexOutOfBoundsException();\n\t\t\t}\n");
								}
								else 
								{
									ret.append("\t\t\t\t__OPEN_A=_");
									ret.append(elementName);
									ret.append(".length;\n\t\t\t\t");
									ret.append(elementType);
									ret.append(" array[] = new ");
									ret.append(elementType);
									ret.append("[__OPEN_A+10];\n\t\t\t\tSystem.arraycopy(_");
									ret.append(elementName);
									ret.append(",0,array,0,_");
									ret.append(elementName);
									ret.append(".length);\n\t\t\t\t_");
									ret.append(elementName);
									ret.append(" = array;\n\t\t\t}\n");
								}
								ret.append("\t\t\t_");
								ret.append(elementName);
								ret.append("[__OPEN_A] = newValue; \n\t\t}\n\t\telse \n\t\t{\n\t\t\t_");
								ret.append(elementName);
								ret.append(" = new ");
								ret.append(elementType);
								ret.append("[");
								if (maxOccurs > 0)
									ret.append(std::to_string(maxOccurs));
								else
									ret.append("10");
								ret.append("];\n\t\t\t_");
								ret.append(elementName);
								ret.append("[0] = newValue;\n\t\t}\n");
								ret.append("\t\t_");
								ret.append(elementName);
								ret.append("Count++;\n\t}\n");

							}
							else 
							{
								ret.append("\t");
								ret.append(nativeType);
								ret.append(" get");
								ret.append(elementName);
								ret.append("(int idx) \n\t{ \n\t\treturn _");
								ret.append(elementName);
								ret.append("[idx].");
								ret.append(nativeType);
								ret.append("Value();\n\t}\n\tvoid set");
								ret.append(elementName);
								ret.append("(");
								ret.append(nativeType);
								ret.append(" newValue) \n\t{\n\t\tif(_");
								ret.append(elementName);
								ret.append("!=null) \n\t\t{\n\t\t\tint __OPEN_A=-1;\n\t\t\tfor(int __I_A=0;__I_A<_");
								ret.append(elementName);
								ret.append(".length;__I_A++) \n\t\t\t{\n\t\t\t\tif(_");
								ret.append(elementName);
								ret.append("[__I_A]==null) \n\t\t\t\t{\n\t\t\t\t\t__OPEN_A=__I_A;\n\t\t\t\t\tbreak;\n\t\t\t\t}\n");
								ret.append("\t\t\t}\n\t\t\tif(__OPEN_A<0) {\n");
								if (maxOccurs > 0) 
								{
									ret.append("\t\t\t\tthrow new IndexOutOfBoundsException();\n\t\t\t}\n");
								}
								else 
								{
									ret.append("\t\t\t\t__OPEN_A=_");
									ret.append(elementName);
									ret.append(".length;\n\t\t\t\t");
									ret.append(elementType);
									ret.append(" array[] = new ");
									ret.append(elementType);
									ret.append("[__OPEN_A+10];\n\t\t\t\tSystem.arraycopy(_");
									ret.append(elementName);
									ret.append(",0,array,0,_");
									ret.append(elementName);
									ret.append(".length);\n\t\t\t\t_");
									ret.append(elementName);
									ret.append(" = array;\n\t\t\t}\n");
								}
								
								ret.append("\t\t\t_");
								ret.append(elementName);
								ret.append("[__OPEN_A] = new ");
								ret.append(elementType);
								ret.append("(newValue);\n\t\t}\n\t\telse {\n\t\t\t_");
								ret.append(elementName);
								ret.append(" = new ");
								ret.append(elementType);
								ret.append("[");
								if (maxOccurs > 0)
									ret.append(std::to_string(maxOccurs));
								else
									ret.append("10");
								ret.append("];\n\t\t\t_");
								ret.append(elementName);
								ret.append("[0] = ");
								ret.append(elementType);
								ret.append(".valueOf(newValue);\n\t\t}\n\t}\n");
							}
						}
						else 
						{
							std::string  nativeType = convertToNativeType(elementType);
							if (nativeType.empty()
								|| XmlSchemaLoader::bObjectAccessors) 
							{
								ret.append("\t");
								ret.append(elementType);
								ret.append(" get");
								ret.append(elementName);
								ret.append("() \n\t{ \n\t\treturn ");
								ret.append("_");
								ret.append(elementName);
								ret.append(";\n\t}\n");
								ret.append("\tvoid set");
								ret.append(elementName);
								ret.append("(");
								ret.append(elementType);
								ret.append(" newValue)\n\t{ \n\t\t");
								ret.append("_");
								ret.append(elementName);
								ret.append(" = newValue;\n\t}\n");
							}
							else 
							{
								ret.append("\t");
								ret.append(nativeType);
								ret.append(" get");
								ret.append(elementName);
								ret.append("() \n\t{ \n\t\treturn _");
								ret.append(elementName);
								ret.append(".");
								ret.append(nativeType);
								ret.append("Value();\n\t}\n\tvoid set");
								ret.append(elementName);
								ret.append("(");
								ret.append(nativeType);
								ret.append(" newValue)\n\t{ \n\t\t_");
								ret.append(elementName);
								ret.append(" = ");
								ret.append(elementType);
								ret.append(".valueOf(newValue);\n\t}\n");
							}
						}
					}
				}
			}

			for (auto & a : attributes) 
			{
				if (a != nullptr) 
				{
					a->generateCode(ret);
				}
			}

			if (bContent) 
			{
				bConstructor = false;
				ret.append("\tint getContentElementsSize() \n\t{\n\t\treturn _contentElements.size();\n\t}\n");
				ret.append("\tEnumeration getContentElements() \n\t{\n\t\treturn _contentElements.elements();\n\t}\n");
				ret.append("\tObject getContentElements(int i) \n\t{\n\t\treturn _contentElements.get(i);\n\t}\n");
				ret.append("\tvoid setContentElements(Object newValue) \n\t{\n\t\t_contentElements.add(newValue);\n\t}\n");
			}

			if (false) 
			{
				ret.append("\tvoid writeExternal(ObjectOutput out) {\n");
				generateWrite(ret);
				for (auto & a : attributes) 
				{
					if (a != nullptr) 
					{
						a->generateWrite(ret);
					}
				}
				ret.append("\t}\n");

				ret.append("\tvoid readExternal(ObjectInput in)\n\t{\n");
				generateRead(ret);
				for (auto & a : attributes) 
				{
					if (a != nullptr) 
					{
						a->generateRead(ret);
					}
				}
				ret.append("\t}\n");
			}

			if (cacheable) 
			{

				ret.append("\tDATAARRAY * write()\n\t{\n");
				ret.append("\t\tint sa_idx = 0;\n");
				ret.append("\t\tDATAARRAY * da = DataArray(DATATYPEENUM::DT_DATAITEM, ");
				ret.append(std::to_string(elements.size() + attributes.size()));
				ret.append(");\n");
				generateCacheWrite(ret);
				for (auto & a : attributes) 
				{
					if (a != nullptr) 
					{
						a->generateCacheWrite(ret);
					}
				}
				ret.append("\t\tDataItemClear(&item);\n");
				ret.append("\t\treturn da;\n");
				ret.append("\t}\n");

				ret.append("\tvoid read(DATAARRAY * in)\n\t{\n");
				ret.append("\t\tint sa_idx = 0;\n");
				
				if (bHasNonJava || bHasArrays) 
				{
					ret.append("\t\tDATAARRAY daItem;\n");
				}
				if (bHasNonJavaArray) 
				{
					ret.append("\t\tDATAARRAY daElement\n");
				}

				generateCacheRead(ret);
				for (auto & a : attributes) 
				{
					if (a != nullptr) 
					{
						a->generateCacheRead(ret);
					}
				}

				ret.append("\t}\n");
				/*
				ret.append("\tvoid writeObject(byte [] data)\n\t{\n");
				ret.append("\t\titem.writeObject(data);\n");
				ret.append("\t\tDATAARRAY in\n");
				ret.append("\t\titem.getDataArray(in);\n");
				ret.append("\t\treadCache(in);\n\t}\n");

				ret.append("\tbyte [] readObject()\n\t{\n");
				ret.append("\t\tDATAARRAY requestDA = writeCache();\n");
				ret.append("\t\titem.putDataArray(requestDA);\n");
				ret.append("\t\treturn item.readObject();\n\t}\n");
				*/
			}

			if (elements.size() > 0)
			{
				ret.append("private:\n");
			}

			for (auto & e : elements)
			{
				std::string elementName = e->getElementType();
				elementName = nameMangler.encode(elementName);
				std::string  elementType = e->getElementTypeTag();
				if (!elementName.empty())
				{
					ret.append("\t");
					if (e->getMaxOne())
					{
						ret.append(elementType);
						ret.append(" _");
						ret.append(elementName);
						getInitInfo(elementType, e);
					}
					else
					{
						ret.append(elementType);
						ret.append(" _");
						ret.append(elementName);
						ret.append("[] = ");
						if (!e->getOptional())
						{
							int maxOccurs = e->getMaxOccurs();
							if (maxOccurs == 0)
							{
								ret.append("new ");
								ret.append(elementType);
								ret.append("[10];\n");
							}
							else
							{
								ret.append("new ");
								ret.append(elementType);
								ret.append("[");
								ret.append(std::to_string(maxOccurs));
								ret.append("];\n");
							}
						}
						else
						{
							ret.append("null;\n");
						}
						ret.append("\tint ");
						ret.append("_");
						ret.append(elementName);
						ret.append("Count = 0;\n");
					}
				}
			}

			for (auto & a : attributes) 
			{
				if (a != nullptr)
					a->generateProperty(ret);
			}

			if (bContent) 
			{
				ret.append("vector _contentElements;\n");
			}
			if (bText) 
			{
				const std::string & type = getProperty("type");
				if (!type.empty()) 
				{
					std::string  cType = convertType(type);
					cType = nameMangler.encode(cType);
					ret.append(cType);
					ret.append(" _contentData;\n");
				}
				else 
				{
					ret.append("string  _contentData;\n");
				}
			}
			if (cacheable) 
			{
				ret.append("\tDATAITEM item;\n");
			}
			ret.append("};\n");
		}
	}
	catch (std::exception e) 
	{
		std::cout << e.what();
	}
	return ret;
}

void ElementType::generateWrite(std::string & ret) const
{
	try 
	{
		for (auto & e : elements) 
		{
			if (e != nullptr) 
			{
				NameMangler nameMangler;
				const std::string & elementName = nameMangler.encode(e->getElementType());
				if (!elementName.empty())
				{
					std::string method;
					const std::string & elementType = e->getElementTypeTag();
					auto & it = writeMethods.find(elementType);
					if (it != writeMethods.end())
					{
						method = (*it).second;
					}

					if (method.empty())
						method = "out.writeObject(";
					ret.append("\t\t");
					if (e->getMaxOne())
					{
						ret.append(method);
						ret.append("_");
						ret.append(elementName);
						ret.append(");\n");
					}
					else
					{
						ret.append("out.writeInt(");
						ret.append("_");
						ret.append(elementName);
						ret.append("Count);\n\t\tif(_");
						ret.append(elementName);
						ret.append("!=null) \n\t\t{\n");
						ret.append("\t\t\tfor(int __I_A=0;__I_A<_");
						ret.append(elementName);
						ret.append("Count;__I_A++) \n\t\t{\n");
						ret.append("\t\t\t\tif(_");
						ret.append(elementName);
						ret.append("[__I_A]==null) \n\t\t{\n\t\t\t\t\tbreak;\n\t\t\t\t}\n");
						ret.append("\t\t\t\tout.writeObject(_");
						ret.append(elementName);
						ret.append("[__I_A]);\n\t\t\t}\n\t\t}\n");
					}
				}
			}
		}
	}
	catch (std::exception e) 
	{
		std::cout << e.what() << std::endl;
	}
}

void ElementType::generateRead(std::string & ret) const
{
	try 
	{
		for (auto & e : elements) 
		{
			if (e != nullptr) 
			{
				NameMangler nameMangler;
				std::string elementName = nameMangler.encode(e->getElementType());
				if (!elementName.empty())
				{
					std::string method;
					const std::string & elementType = e->getElementTypeTag();
					auto & it = readMethods.find(elementType);
					if (it != readMethods.end())
					{
						method = (*it).second;
					}
					if (method.empty())
						method = "in.readObject(";
					ret.append("\t\t");
					if (e->getMaxOne())
					{
						ret.append("_");
						ret.append(elementName);
						ret.append(" = (");
						ret.append(elementType);
						ret.append(")");
						ret.append(method);
						ret.append(");\n");
					}
					else
					{
						ret.append("_");
						ret.append(elementName);
						ret.append("Count = in.readInt();\n");
						ret.append("\t\tif(_");
						ret.append(elementName);
						ret.append("Count>0) \n\t\t{\n");
						ret.append("\t\t\t_");
						ret.append(elementName);
						ret.append(" = new ");
						ret.append(elementType);
						ret.append(" [_");
						ret.append(elementName);
						ret.append("Count];\n");
						ret.append("\t\t\tfor(int __I_A=0;__I_A<_");
						ret.append(elementName);
						ret.append("Count;__I_A++) \n\t\t{\n\t\t\t\t_");
						ret.append(elementName);
						ret.append("[__I_A] = (");
						ret.append(elementType);
						ret.append(")in.readObject();\n\t\t\t}\n\t\t}\n");
					}
				}
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void ElementType::generateCacheWrite(std::string & ret) const
{
	try 
	{
		for (auto & e : elements) 
		{
			if (e != nullptr)
			{
				NameMangler nameMangler;
				std::string elementName = nameMangler.encode(e->getElementType());
				if (!elementName.empty())
				{
					std::string type;
					const std::string & elementType = e->getElementTypeTag();
					auto & it = cacheTypes.find(elementType);
					if (it != cacheTypes.end())
					{
						type=(*it).second;
					}

					bool isJava = true;
					if (type.empty())
					{
						type = "DATAITEM";
						isJava = false;
					}
					if (e->getMaxOne())
					{
						if (isJava)
						{
							ret.append("\t\titem.put");
							ret.append(type);
							ret.append("(_");
							ret.append(elementName);
							ret.append(");\n");
						}
						else
						{
							ret.append("\t\titem.putDataArray(_");
							ret.append(elementName);
							ret.append(".writeCache());\n");
						}
					}
					else
					{
						ret.append("\t\tif(_");
						ret.append(elementName);
						ret.append("!=null) {\n");
						ret.append("\t\t\tDataArray daChild = new DataArray(DATAITEM::DATAITEMARRAY, _");
						ret.append(elementName);
						ret.append("Count);\n");
						ret.append("\t\t\tfor(int __I_A=0;__I_A<_");
						ret.append(elementName);
						ret.append("Count;__I_A++) {\n\t\t\t\t");
						if (isJava)
						{
							ret.append("daChild.set");
							ret.append(type);
							ret.append("(__I_A, _");
							ret.append(elementName);
							ret.append("[__I_A]);\n\t\t\t}\n");
							ret.append("\t\t\titem.putDataArray(daChild);\n");
						}
						else
						{
							ret.append("daChild.setDataArray(__I_A, _");
							ret.append(elementName);
							ret.append("[__I_A].writeCache());\n\t\t\t}\n");
							ret.append("\t\t\titem.putDataArray(daChild);\n");
						}
						ret.append("\t\t}\n\t\telse {\n\t\t\titem.putEmpty();\n\t\t}\n");
					}
					ret.append("\t\tda.setDataItem(sa_idx++, item);\n");
				}
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void ElementType::generateCacheRead(std::string & ret) const
{
	try 
	{
		for (auto & e : elements) 
		{
			if (e != nullptr)
			{
				NameMangler nameMangler;
				std::string  elementName = nameMangler.encode(e->getElementType());
				if (!elementName.empty())
				{
					std::string type;
					ret.append("\t\tin.getDataItem(sa_idx++, item);\n");
					const std::string & elementType = e->getElementTypeTag();
					auto & it = cacheTypes.find(elementType);
					if (it != cacheTypes.end())
					{
						type = (*it).second;
					}
					bool isJava = true;
					if (type.empty())
					{
						type = "DATAITEM";
						isJava = false;
					}

					if (e->getMaxOne())
					{
						if (isJava)
						{
							ret.append("\t\t_");
							ret.append(elementName);
							ret.append(" = item.get");
							ret.append(type);
							ret.append("();\n");
						}
						else
						{
							ret.append("\t\titem.getDataArray(daItem);\n\t\t_");
							ret.append(elementName);
							ret.append(".readCache(daItem);\n");
						}
					}
					else
					{
						ret.append("\t\titem.getDataArray(daItem);\n");
						ret.append("\t\t_");
						ret.append(elementName);
						ret.append("Count = daItem.getElements();\n");
						ret.append("\t\tif(_");
						ret.append(elementName);
						ret.append("Count>0) {\n");
						ret.append("\t\t\t_");
						ret.append(elementName);
						ret.append(" = new ");
						ret.append(elementType);
						ret.append(" [_");
						ret.append(elementName);
						ret.append("Count];\n");
						ret.append("\t\t\tfor(int __I_A=0;__I_A<_");
						ret.append(elementName);
						ret.append("Count;__I_A++) {\n\t\t\t\t_");
						ret.append(elementName);
						ret.append("[__I_A] = new ");
						ret.append(elementType);
						ret.append("(");
						if (isJava)
						{
							ret.append("daItem.get");
							ret.append(type);
							ret.append("(__I_A));\n");
						}
						else
						{
							ret.append(");\n\t\t\t\tdaItem.getDataArray(__I_A, daElement);\n\t\t\t\t_");
							ret.append(elementName);
							ret.append("[__I_A].readCache(daElement);\n");
						}
						ret.append("\t\t\t}\n\t\t}\n");
					}
				}
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

bool ElementType::isJavaType(const std::string & className)
{
	bool bRet = false;
	if (className!="java.")
	{
		bRet = true;
	}
	else 
	{
		if (className!="string")
			bRet = true;
		else if (className=="bool")
			bRet = true;
		else if (className=="byte")
			bRet = true;
		else if (className=="char")
			bRet = true;
		else if (className=="double")
			bRet = true;
		else if (className=="float")
			bRet = true;
		else if (className=="int")
			bRet = true;
		else if (className=="long")
			bRet = true;
		if (className=="short")
			bRet = true;
	}
	return bRet;
}

void ElementType::getInitInfo(const std::string & className, std::shared_ptr<ElementData> & e) const
{
	try {
		bool bOptional = e->getOptional();
		std::string defaultValue = e->getProperty("default");

		if (!defaultValue.empty()) 
		{
			if (className=="string" || className=="char") 
			{
				defaultValue = '"' + e->getProperty("default") + '"';
			}
			else if (className=="Boolean") 
			{
				defaultValue = !(defaultValue == "1") && equalsIgnoreCase(defaultValue, "true");
			}
		}
		ret.append(" = ");
		if (className=="bool")
		{
			if (defaultValue.empty())
				defaultValue = "false";
			ret.append(defaultValue);
		}
		else if (className=="byte")
		{
			if (defaultValue.empty())
				defaultValue = "''";
			ret.append(defaultValue);
		}
		else if (className=="char")
		{
			if (defaultValue.empty())
				defaultValue = "''";
			ret.append(defaultValue);
		}
		else if (className=="double")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(defaultValue);
		}
		else if (className=="float")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(defaultValue);
		}
		else if (className=="int")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(defaultValue);
		}
		else if (className=="long")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(defaultValue);
		}
		else if (className=="short")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(defaultValue);
		}
		else if (bOptional && defaultValue.empty()) 
		{
			ret.append(" null");
		}
		else if (className=="Boolean")
		{
			if (defaultValue.empty())
				defaultValue = "false";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else if (className=="Byte")
		{
			if (defaultValue.empty())
				defaultValue = "''";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else if (className=="Character")
		{
			if (defaultValue.empty())
				defaultValue = "''";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else if (className=="Double")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else if (className=="Float")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else if (className=="Integer")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else if (className=="Long")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else if (className=="Short")
		{
			if (defaultValue.empty())
				defaultValue = "0";
			ret.append(" ");
			ret.append(className);
			ret.append(".valueOf(");
			ret.append(defaultValue);
			ret.append(")");
		}
		else 
		{
			if (defaultValue.empty())
				defaultValue = "";
			ret.append(" new ");
			ret.append(className);
			ret.append("(");
			ret.append(defaultValue);
			ret.append(")");
		}
		ret.append(";\n");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

const std::string ElementType::getName() const
{
	return getProperty("name");
}

const std::string ElementType::convertType(const std::string & inType)
{
	std::string outType;

	try 
	{
		if (!inType.empty()) 
		{
			auto it = dataTypes.find(inType);
			outType = (*it).second;
		}

		if (outType.empty())
		{
			outType = "string";
		}
	}
	catch (std::exception e) 
	{
		std::cout << e.what() << std::endl;
	}

	return outType;
}

const std::string ElementType::convertToNativeType(const std::string & inType)
{
	std::string outType;

	try 
	{
		if (!inType.empty())
		{
			auto & it = javaTypes.find(inType);
			if (it != javaTypes.end())
				outType = (*it).second;
		}
	}
	catch (std::exception e) 
	{
		std::cout << e.what() << std::endl;
	}

	return outType;
}

void ElementType::insertType(const std::string & simpleType, const std::string & xmlType)
{
	try 
	{
		dataTypes.insert(std::make_pair(xmlType, convertType(xmlType)));
	}
	catch (std::exception e) 
	{
		std::cout << e.what() << std::endl;
	}
}

