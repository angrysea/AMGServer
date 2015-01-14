#include "XMLElementSerializer.h"

XMLElementSerializer::XMLElementSerializer(const std::string & name) :
	XmlBase(),
	name{ name },
	data{ nullptr },
	len{ 0 }
{
}

XMLElementSerializer::XMLElementSerializer(const XMLElementSerializer & aCopy) :
	XmlBase(aCopy),
	name{ aCopy.name },
	len{ aCopy.len }
{
}

XMLElementSerializer::XMLElementSerializer(const XMLElementSerializer && aCopy) :
	XmlBase(aCopy),
	name{ std::move(aCopy.name) },
	len{ aCopy.len }
{
	len = 0;
	delete[] data;
	data = aCopy.data;
}

XMLElementSerializer::~XMLElementSerializer()
{
	delete[] data;
}

void XMLElementSerializer::setData(char *& buf, int offset, int newLen)
{
	len = newLen;
	data = new char[len];
	memcpy(buf + (sizeof(char)*offset), data, (sizeof(char)*len));
}

void XMLElementSerializer::setData(std::string & buf)
{
	len = buf.length() + 1;
	data = new char[len];
	strcpy_s(data, len, buf.substr(0, len).c_str());
}

void XMLElementSerializer::update(std::shared_ptr<XMLElementSerializer > & me, std::map<const std::string,
	std::shared_ptr<XMLElementSerializer>> & map, const std::string & type)
{
	update(me, map, type, false);
}

void XMLElementSerializer::update(std::shared_ptr<XMLElementSerializer > & me, std::map<const std::string, std::shared_ptr<XMLElementSerializer>> & map,
	const std::string & type, bool namerequired)
{
	try
	{
		if (name == type)
		{
			const std::string & n = getProperty("name");
			if (!n.empty() || !namerequired)
			{
				map.insert(std::make_pair(n, me));
			}
		}

		for (auto & e : childElements)
		{
			e->update(e, map, type);
		}
	}
	catch (std::exception e)
	{
		std::cout << "Exception thrown Update error " << e.what();
	}
}

std::vector<std::shared_ptr<XMLElementSerializer>> & XMLElementSerializer::children()
{
	return childElements;
}

const std::string & XMLElementSerializer::getName() const
{
	return name;
}

void XMLElementSerializer::addChild(std::shared_ptr<XMLElementSerializer> & child)
{
	childElements.push_back(child);
}
