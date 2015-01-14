#include <string>
#include <cctype>
#include <algorithm>
#include <functional>

#include "AttributeType.h"
#include "../xmlutils/NameMangler.h"


std::map<std::string, std::string> AttributeType::dataTypes
{
	{ "boolean", "bool" },
	{ "string.ansi", "string" },
	{ "fixed.14.4", "float" },
	{ "float", "float" },
	{ "i4", "int" },
	{ "long", "long" },
	{ "unsignedLong", "unsigned long" },
	{ "short", "short" },
	{ "unsignedShort", "unsigned short" },
	{ "double", "double" },
	{ "date", "Date" },
	{ "dateTime", "Date" },
	{ "dateTime.iso8601", "Date" },
	{ "dateTime.iso8601.tz", "Date" },
	{ "date.iso8601", "Date" },
	{ "time", "Date" },
	{ "time.iso8601", "Date" },
	{ "time.iso8601.tz", "Date" },
	{ "number", "char[]" },
	{ "int", "int" },
	{ "integer", "int" },
	{ "i1", "char" },
	{ "i2", "short" },
	{ "i4", "int" },
	{ "i8", "long" },
	{ "ui1", "unsigned char" },
	{ "ui2", "unsigned short" },
	{ "ui4", "unsigned int" },
	{ "positiveInteger", "int" },
	{ "nonPositiveInteger", "int" },
	{ "negativeInteger", "int" },
	{ "nonNegativeInteger", "int" },
	{ "ui8", "unsigned long" },
	{ "r4", "float" },
	{ "r8", "double" },
	{ "decimal", "double" },
	{ "float.IEEE.754.32", "float" },
	{ "float.IEEE.754.64", "double" },
	{ "char", "char" },
	{ "bin.hex", "char[]" },
	{ "uri", "string" },
	{ "uuid", "string" },
	{ "uriReference", "string" }
};

const std::map<std::string, std::string>  AttributeType::cacheTypes
{
	{ "boolean", "boolVal" },
	{ "string", "sstringVal" },
	{ "float", "fltVal" },
	{ "int", "intVal" },
	{ "long", "lVal" },
	{ "short", "iVal" },
	{ "double", "dblVal" },
	{ "Date", "date" },
	{ "byte", "bVal" },
	{ "char", "cVal" }
};

const std::map<std::string, std::string>  AttributeType::writeMethods
{
	{ "boolean", "out.writeBoolean(" },
	{ "char[]", "writeString(out, " },
	{ "float", "out.writeFloat(" },
	{ "int", "out.writeInt(" },
	{ "long", "out.writeLong(" },
	{ "short", "out.writeShort(" },
	{ "double", "out.writeDouble(" },
	{ "Date", "writeDate(out, " },
	{ "byte", "out.writeChar(" },
	{ "char", "out.writeChar(" }
};

const std::map<std::string, std::string>  AttributeType::readMethods
{
	{ "boolean", "in.readBoolean(" },
	{ "char[]", "readString(in" },
	{ "float", "in.readFloat(" },
	{ "int", "in.readInt(" },
	{ "long", "in.readLong(" },
	{ "short", "in.readShort(" },
	{ "double", "in.readDouble(" },
	{ "Date", "readDate(in" },
	{ "byte", "in.readChar(" },
	{ "char", "in.readChar(" }
};

void  AttributeType::generateCode(std::string & ret)
{
	NameMangler nameMangler;
	auto & it = properties.find("name");
	if (it != properties.end())
	{
		std::string name = (*it).second;
		int start = name.find_first_of(':');
		if (start != std::string::npos)
		{
			name = name.substr(start + 1);
			name = nameMangler.encode(name);
		}

		std::string type;
		auto it = properties.find("type");
		if (it == properties.end())
		{
			type = "string";
		}
		else
		{
			type = (*it).second;
		}

		type = convertType(type);
		ret.append("\tconst ");
		ret.append(type);
		ret.append(" & get");
		ret.append(name);
		ret.append("() const\n\t{\n\t\treturn ");
		ret.append("_");
		ret.append(name);
		ret.append(";\n\t}\n");
		ret.append("\tvoid set");
		ret.append(name);
		ret.append("(const ");
		ret.append(type);
		ret.append(" & newValue)\n\t{\n\t\t");
		ret.append("_");
		ret.append(name);
		ret.append(" = newValue;\n\t}\n");
	}
}

void  AttributeType::generateProperty(std::string & ret)
{
	try
	{
		NameMangler nameMangler;
		auto & it = properties.find("name");
		if (it != properties.end())
		{
			std::string name = (*it).second;
			int start = name.find(':');
			if (start != std::string::npos)
			{
				name = name.substr(start + 1);
			}

			name = nameMangler.encode(name);

			std::string type;
			auto it = properties.find("type");
			if (it == properties.end())
			{
				type = "string";

			}
			else
			{
				type = (*it).second;
			}

			type = convertType(type);
			type = nameMangler.encode(type);

			ret.append("\t");
			std::string defaultValue = getMemberDefault(type);
			ret.append(type);
			ret.append(" _");
			ret.append(name);

			if (!defaultValue.empty() && defaultValue.length()>0)
			{
				ret.append(" = ");
				ret.append(defaultValue);
			}
			ret.append(";\n");
		}
	}
	catch (std::exception t)
	{
	}
}

const std::string AttributeType::getMemberDefault(const std::string & type)
{
	std::string ret;
	std::string defaultValue;
	auto it = properties.find("default");
	if (it != properties.end())
	{
		defaultValue = (*it).second;
		ret.append(defaultValue);
		if (type == "bool" && !defaultValue.empty())
		{
			if (defaultValue == "1")
			{
				ret.append("true");
			}
			else
			{
				ret.append(defaultValue);
			}
		}
	}
	return ret;
}

std::string AttributeType::convertType()
{
	std::string inType;
	auto it = properties.find("type");
	if (it != properties.end())
	{
		inType = (*it).second;
		inType = convertType(inType);
	}
	return inType;
}

std::string AttributeType::convertType(const std::string & inType)
{
	std::string outType;
	try 
	{
		if (!inType.empty())
		{
			int colon = inType.find(':');
			std::string type;
			if (colon == std::string::npos)
			{
				type = inType;
			}
			else
			{
				type = inType.substr(colon + 1);
			}

			auto & it = dataTypes.find(type);
			if (it != dataTypes.end())
			{
				outType = (*it).second;
			}
		}

		if (outType.empty())
		{
			outType = "string";
		}
	}
	catch (std::exception e) 
	{
	}

	return outType;
}

void AttributeType::insertType(const std::string & simpleType, const std::string & xmlType)
{
	try 
	{
		dataTypes.insert(std::make_pair(xmlType, convertType(xmlType)));
	}
	catch (std::exception e) 
	{
	}
}

bool AttributeType::isString(const std::string & type)
{
	return type=="string";
}

void AttributeType::generateWrite(std::string & ret)
{
	try 
	{
		NameMangler nameMangler;
		std::string name;
		auto it = properties.find("name");
		if (it != properties.end()) 
		{
			name = (*it).second;
			int start = name.find(':');
			if (start != std::string::npos)
			{
				name = name.substr(start + 1);
			}

			name = nameMangler.encode(name);

			std::string inType;
			auto it = properties.find("type");
			if (it == properties.end())
			{
				inType = "string";
			}
			else
			{
				inType = (*it).second;
			}
			
			inType = convertType(inType);
			ret.append("\t\t");

			auto itWriteMethods = writeMethods.find(inType);
			if (itWriteMethods == writeMethods.end())
			{
				ret.append("out.writeObject(");
			}
			else 
			{
				ret.append((*itWriteMethods).second);
			}
			ret.append("_");
			ret.append(name);
			ret.append(");\n");
		}
	}
	catch (std::exception t) 
	{
	}
}

void AttributeType::generateRead(std::string & ret)
{
	try 
	{
		std::string name;
		NameMangler nameMangler;
		auto it = properties.find("name");
		if (it != properties.end())
		{
			name = (*it).second;
			
			int start = name.find(':');
			if (start != std::string::npos)
			{
				name = name.substr(start + 1);
			}

			name = nameMangler.encode(name);
			
			std::string inType;
			auto it = properties.find("type");
			if (it == properties.end())
			{
				inType = "string";
			}
			else
			{
				inType = (*it).second;
			}

			ret.append("\t\t_");
			ret.append(name);
			ret.append(" = ");
			auto itReadMethods = readMethods.find(inType);
			if (itReadMethods == readMethods.end())
			{
				ret.append("(");
				ret.append(inType);
				ret.append(")");
				ret.append("in.readObject(");
			}
			else 
			{
				ret.append((*itReadMethods).second);
			}
			ret.append(");\n");
		}
	}
	catch (std::exception t) 
	{
	}
}

void AttributeType::generateCacheWrite(std::string & ret)
{
	try 
	{
		std::string name;
		NameMangler nameMangler;
		auto it = properties.find("name");
		if (it != properties.end())
		{
			name = (*it).second;

			int start = name.find(':');
			if (start != std::string::npos)
			{
				name = name.substr(start + 1);
			}

			name = nameMangler.encode(name);

			std::string inType;
			auto it = properties.find("type");
			if (it == properties.end())
			{
				inType = "string";
			}
			else
			{
				inType = (*it).second;
			}

			start = inType.find(':');
			if (start != std::string::npos)
			{
				inType = inType.substr(start + 1);
			}
			auto itCacheTypes = cacheTypes.find(inType);
			std::string type;
			if (itCacheTypes == cacheTypes.end())
			{
				type = "sstringVal";
			}
			else
			{
				type = (*itCacheTypes).second;
			}
			ret.append("\t\titem.");
			ret.append(type);
			if (type == "sstringVal")
			{
				ret.append("=");
				ret.append("SafeString(_");
				ret.append(name);
				ret.append(".c_str())");
			}
			else
			{
				ret.append("=_");
				ret.append(name);
			}
			ret.append(";\n");
			ret.append("\t\tDataArrayPutElement(da, sa_idx++, &item);\n");
		}
	}
	catch (std::exception t) 
	{
	}
}

void AttributeType::generateCacheRead(std::string & ret)
{
	try 
	{
		std::string name;
		auto it = properties.find("name");
		if (it != properties.end())
		{
			name = (*it).second;

			int start = name.find(':');
			if (start != std::string::npos)
			{
				name = name.substr(start + 1);
			}

			NameMangler nameMangler;
			name = nameMangler.encode(name);

			ret.append("\t\tDataArrayGetElement(in, sa_idx++, &item);\n");

			std::string inType;
			auto it = properties.find("type");
			if (it == properties.end())
			{
				inType = "string";
			}
			else
			{
				inType = (*it).second;
			}

			start = inType.find(':');
			if (start != std::string::npos)
			{
				inType = inType.substr(start + 1);
			}
			auto itCacheTypes = cacheTypes.find(inType);
			std::string type;
			if (itCacheTypes == cacheTypes.end())
			{
				type = "sstringVal";
			}
			else
			{
				type = (*itCacheTypes).second;
			}
			ret.append("\t\t_");
			ret.append(name);
			ret.append(" = item.");
			ret.append(type);
			ret.append(";\n");
		}
	}
	catch (std::exception t) 
	{
	}
}
