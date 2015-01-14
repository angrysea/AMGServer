#pragma once 

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

#include "export.h"
#include "XmlBase.h"

class XMLCONVERTER_API XMLElementSerializer : public XmlBase
{
public:
	XMLElementSerializer(const std::string & name);
	XMLElementSerializer::XMLElementSerializer(const XMLElementSerializer & aCopy);
	XMLElementSerializer::XMLElementSerializer(const XMLElementSerializer && aCopy);
	virtual ~XMLElementSerializer();
	void setData(char *& buf, int offset, int newLen);
	void setData(std::string & buf);
	void update(std::shared_ptr<XMLElementSerializer > & me, std::map<const std::string, std::shared_ptr<XMLElementSerializer>> & map, const std::string & type);
	void update(std::shared_ptr<XMLElementSerializer > & me, std::map<const std::string, 
		std::shared_ptr<XMLElementSerializer>> & map, const std::string & type, bool namerequired);
	std::vector<std::shared_ptr<XMLElementSerializer>> & children();
	const std::string & getName() const;
	void addChild(std::shared_ptr<XMLElementSerializer> & child);

private:
	std::vector<std::shared_ptr<XMLElementSerializer>> childElements;
	std::string name;
	char * data;
	int len;
};
