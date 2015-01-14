#include "Registery.h"
#include "RegisteryNode.h"
#include "RegisterySpider.h"

Registery * Registery::registery = 0;

Registery::Registery(std::string & fileName) :
	fileName(fileName),
	root(0),
	current(0)
{
}

int Registery::openRegistery() {

	regFile.openRead(fileName);

	while (!regFile.eof())
	{
		std::string wline;
		regFile.getLine(wline);
		if(root==0) 
		{
			std::string::size_type start = 
				wline.find_first_not_of("Windows Registery Editor Version ");
			if(start>0)
			{
				int version = atoi(wline.substr(start).c_str());
				std::cout << "Version - " << version << std::endl;
			}
			root = new RegisteryNode(0, "Root", wline);
		}
		else
		{
			if(wline.length()>0)
			{
				std::string::size_type key = wline.find_first_of("[");
				if(key == 0)
				{
					std::string::size_type end = wline.find_first_of("]")-1;
					insertKey(const_cast<char*>(wline.substr(1,end).c_str()));
				}
				else
				{
					insertValue(wline);
				}
			}
		}
	}

	regFile.close();

	return 0;
}

int Registery::saveRegistery() 
{
	regFile.openWrite( fileName );
	RegisterySpider spider;
	spider.save(root, "", regFile);
	regFile.close();

	return 0;
}

void Registery::insertKey(char * regLine)
{
	char * next = 0;
	char * token = 0;
	std::string key;
	current = root;

	int size = strlen(regLine);
		
	token = strtok_s(regLine, "\\", &next);

	while(token!=0) 
	{
		RegisteryNode * tmp = current->findNode(token);
		if(tmp!=0)
		{
			current=tmp;
			std::wcout << "Found Key: " << token << std::endl;
		}
		else
		{
			RegisteryNode * node = new RegisteryNode(current, token);
			current->insert(node);
			std::wcout << "New Key: " << token << std::endl;
			current = node;
		}
		token = strtok_s(0, "\\", &next);
	}
}

void Registery::insertValue(std::string & regLine)
{
	std::string name, type, value;

	if(regLine[0]=='@')
	{
		name = "@";
		regLine = regLine.substr(1);
	}
	else
	{
		regLine = regLine.substr(1);
		std::string::size_type end = regLine.find_first_of('"');
		name = regLine.substr(0, end);
		regLine = regLine.substr(end);
	}

	regLine = regLine.substr(regLine.find_first_of("=")+1);

	if(regLine[0]=='"')
	{
		if(regLine[1]=='{' && regLine[regLine.length()-2]=='}')
		{
			type = "GUID";
		}
		else
		{
			type = "TEXT";
		}
		regLine = regLine.substr(1);

		std::string::size_type end = regLine.find_last_of('"');
		value = regLine.substr(0, end);
	}
	else
	{
		std::string::size_type end = regLine.find_first_of(':');
		type = regLine.substr(0, end);
		regLine = regLine.substr(end+1);
		value = regLine.substr(0,regLine.length());
	}

	current->insert(new RegisteryNode(current, name, value, type, RegisteryNode::VALUE));
	std::cout << "Add value: " << name << " Type: " << type << " Value: " << value << std::endl;
}

