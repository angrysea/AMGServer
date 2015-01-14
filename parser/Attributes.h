#pragma once

#include <string>
#include <memory>
#include <vector>

#include "export.h"

class PARSER_API AttributeEntry
{
public:
	std::string uri;
	std::string localName;
	std::string qName;
	std::string type;
	std::string value;
};

class PARSER_API  Attributes
{
public:
	Attributes() = default;

	void addAttribute(const std::string &uri, const std::string & localName, 
		const std::string & qName, const std::string & type, const std::string & value)
	{
		std::shared_ptr<AttributeEntry> entry(new AttributeEntry);
		entry->uri = uri;
		entry->localName = localName;
		entry->qName = qName;
		entry->type = type;
		entry->value = value;
		attributes.push_back(entry);
	}

	void clear() 
	{
		attributes.clear();
	}

	int getIndex(const std::string & qName) 
	{
		int index = -1;
		int count = 0;

		for (auto & entry : attributes)
		{
			if (qName == entry->qName) 
			{
				index = count;
				break;
			}
			count++;
		}
		return index;
	}

	int getIndex(const std::string & uri, const std::string & localName) 
	{
		int index = -1;
		int count = 0;
		for (auto & entry : attributes)
		{
			if (localName == entry->localName &&
				((!uri.empty() && !entry->uri.empty() && uri == entry->uri) ||
					(uri.empty() && entry->uri.empty())))
			{
				index = count;
				break;
			}
			count++;
		}
		return index;
	}

	int getLength() const
	{
		return attributes.size();
	}

	const std::string & getLocalName(int index) const
	{
		auto entry = attributes[index];
		return entry->localName;
	}

	const std::string & getQName(int index) 
	{
		auto entry = attributes[index];
		return entry->qName;
	}

	std::string getType(int index) const
	{
		auto entry = attributes[index];
		return entry->type;
	}

	const std::string getType(std::string qName) const
	{
		for(auto entry : attributes)
		{
			if (qName==entry->qName)
			{
				return entry->type;
			}
		}
		return nullptr;
	}

	const std::string getType(const std::string & uri, const std::string & localName) const
	{
		for (auto entry : attributes)
		{
			if (localName == entry->localName &&
				((!uri.empty() && !entry->uri.empty() && uri == entry->uri) ||
				(uri.empty() && entry->uri.empty())))
			{
				return entry->type;
			}
		}
		return nullptr;
	}

	const std::string & getURI(int index) const
	{
		auto entry = attributes[index];
		return entry->uri;
	}

	const std::string & getValue(int index) const
	{
		auto entry = attributes[index];
		return entry->value;
	}

	const std::string & getValue(const std::string & qName) const
	{
		for (auto entry : attributes)
		{
			if (qName == entry->qName)
			{
				return entry->value;
			}
		}
		return nullptr;
	}

	const std::string getValue(const std::string & uri, const std::string & localName) const
	{
		for (auto entry : attributes)
		{
			if (localName == entry->localName &&
				((!uri.empty() && !entry->uri.empty() && uri == entry->uri) ||
				(uri.empty() && entry->uri.empty())))
			{
				return entry->value;
			}
		}
		return nullptr;
	}

	void removeAttribute(int index) 
	{
		attributes.erase(attributes.begin()+index);
	}

	void setAttribute(const std::string &uri, const std::string & localName,
		const std::string & qName, const std::string & type, const std::string & value)
	{
		std::shared_ptr<AttributeEntry> entry(new AttributeEntry);
		entry->uri = uri;
		entry->localName = localName;
		entry->qName = qName;
		entry->type = type;
		entry->value = value;
		attributes.push_back(entry);
	}

	void setAttributes(std::vector<std::shared_ptr<AttributeEntry>> atts)
	{
		attributes.reserve(atts.size());
		copy(atts.begin(), atts.end(), back_inserter(attributes));
	}

	void setLocalName(int index, std::string localName) 
	{
		auto entry = attributes[index];
		entry->localName = localName;
	}

	void setQName(int index, std::string qName) 
	{
		auto entry = attributes[index];
		entry->qName = qName;
	}

	void setType(int index, std::string type) 
	{
		auto entry = attributes[index];
		entry->type = type;
	}

	void setURI(int index, std::string uri) 
	{
		auto entry = attributes[index];
		entry->uri = uri;
	}

	void setValue(int index, std::string value) 
	{
		auto entry = attributes[index];
		entry->value = value;
	}

private:
	std::vector<std::shared_ptr<AttributeEntry>> attributes;

};
