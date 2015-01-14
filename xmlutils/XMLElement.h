#pragma once

#include <string>
#include <map>
#include <memory>



 class XMLElement 
 {
	const std::string & getBeginEvent() 
	{
		return beginEvent;
	}

	const std::string & getDataEvent() 
	{
		return dataEvent;
	}

	const std::string & getEndEvent() 
	{
		return endEvent;
	}

	void setBeginEvent(const std::string & strBeginEvent) 
	{
		if (!strBeginEvent.empty()) 
		{
			bHasEvent = true;
			beginEvent = strBeginEvent;
		}
	}

	void setDataEvent(const std::string & strDataEvent) 
	{
		if (!strDataEvent.empty()) 
		{
			bHasEvent = true;
			dataEvent = strDataEvent;
		}
	}

	void setEndEvent(const std::string & strEndEvent) 
	{
		if (!strEndEvent.empty()) 
		{
			bHasEvent = true;
			endEvent = strEndEvent;
		}
	}

	void put(const std::string & sName, std::shared_ptr<XMLElement> & element) 
	{
		if (debug == true) 
		{
			element->name = sName;
		}
		tags.insert(std::make_pair(sName, element));
	}

	std::shared_ptr<XMLElement> & get(const std::string & sName)
	{
		return get(sName, 99999);
	}

	std::shared_ptr<XMLElement> & get(const std::string & sName, int level)
	{
		std::shared_ptr<XMLElement> element;

		if (sName==name) 
		{
			element = std::shared_ptr<XMLElement>(this);
		}
		else
		{
			auto & it = tags.find(sName);
			if (it != tags.end())
			{
				element = (*it).second;
			}
			else
			{
				if (level > 0)
				{
					for (auto & p : tags)
					{
						element = p.second->get(sName, --level);
						if (element!=nullptr)
						{
							break;
						}
					}
				}
			}
		}
		return element;
	}

	void clear() 
	{
		tags.clear();
	}

	bool hasEvent() 
	{
		return bHasEvent;
	}

private:
	std::map<const std::string &, std::shared_ptr<XMLElement>> tags;
	std::string name;
	std::string beginEvent;
	std::string dataEvent;
	std::string endEvent;
	bool bHasEvent = false;
	bool debug = true;
};