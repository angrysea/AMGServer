#include "EnumType.h"

std::map<std::string, std::string> EnumType::dataTypes;

const std::string & EnumType::generateCode() const
{
	std::string base;
	try 
	{
		std::string name = getProperty("name");
		if (!name.empty()) 
		{
			int size = 0;
			NameMangler nameMangler;
			name = nameMangler.encode(name);
			base = getProperty("base");
			base = nameMangler.encode(base);
			ret.append("import java.io.Serializable;\n");
			ret.append("\nimport java.util.*;\n");
			ret.append(
				"\nclass ");
			ret.append(name);
			ret.append(" implements Serializable ");
			if (!base.empty())
			{
				ret.append(", extends ");
				ret.append(base);
			}
			ret.append(" {\n");

			size = enumvalues.size();
			if (size > 0) 
			{
				ret.append("\tstatic {\n");
				for (auto & v : enumvalues)
				{
					if (!v.empty())
					{
						ret.append("\tvalidValues.add(");
						ret.append(name);
						ret.append(");\n");
					}
				}

				ret.append("\t}\n");
			}
			ret.append("\tconst std::string & getContentData() {\n\t\treturn _contentData;\n\t}\n");
			ret.append("\tvoid setContentData(const std::string & newValue) {\n\t\t_contentData = newValue;\n\t}\n");
			ret.append("\tconst std::string & _contentData;\n");
			ret.append("}\n");
		}
	}
	catch (std::exception e) 
	{
		std::cout << e.what();
	}
	return ret;
}

