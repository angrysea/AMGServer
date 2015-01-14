#pragma once

#include <algorithm>
#include <string>
#include <map>
#include <memory>

#include "export.h"
#include "ElementType.h"

class XMLCONVERTER_API NameProcessor
{
public:

	NameProcessor() = default;

	void setProperty(std::shared_ptr<ElementType> elementType, const std::string & name) 
	{
		std::string temp;
		if (name.find('-') != std::string::npos) 
		{
			temp = name;
			std::replace(name.begin(), name.end(), '-', '_');
			nametable.insert(std::make_pair(name, temp));
		}
		elementType->setProperty("name", name);
	}

	const std::string & generateCode() 
	{
		std::string ret;
		if (nametable.size() > 0) 
		{
			ret.append("\n\n\tpublic String getElementName(String name) {\n");
			ret.append("\t\treturn (String)nametable.get(name);\n\t}\n\n");
			ret.append("\tstatic private Hashtable nametable = new Hashtable();\n\n");
			ret = ret.append("\tstatic {\n");

			for(auto & p : nametable)
			{
				ret.append("\t\tnametable.put(");
				ret.append(p.first);
				ret.append(", ");
				ret.append(p.second);
				ret.append(");\n");
			}
			ret.append("\t}\n\n");
		}
		return ret;
	}

private: 
	std::map<std::string, std::string> nametable;
};
