#pragma once 

#include <string>
#include <map>
#include <memory>

class AliasProcessor 
{
public:
	static std::map<std::string, std::shared_ptr<AliasProcessor>> & getClassMap() 
	{
		return classMap;
	}
	
	static void setAlias(const std::string & className, const std::string & prop, const std::string & alias) 
	{
		auto & it = classMap.find(className);
		
		if (it == classMap.end())
		{
			std::shared_ptr<AliasProcessor> aliasProcessor(new AliasProcessor());
			classMap.insert(std::make_pair(className, aliasProcessor));
			aliasProcessor->addAlias(prop, alias);
		}
		else
		{
			auto & aliasProcessor = (*it).second;
			aliasProcessor->addAlias(prop, alias);
		}
	}
	
	static const std::string & getAlias(const std::string & className, const std::string & prop) 
	{
		std::string alias;
		auto & it = classMap.find(className);
		if (it != classMap.end())
		{ 
			auto & aliasProcessor = (*it).second;
			alias = aliasProcessor->getAlias(prop);
		}
		return alias.empty()?prop:alias;
	}
	
	static const std::string & getProperty(const std::string & className, const std::string & alias) 
	{
		std::string prop;
		auto & it = classMap.find(className);
		if (it!=classMap.end()) 
		{
			auto & aliasProcessor = (*it).second;
			prop = aliasProcessor->getProperty(alias);
		}
		return prop.empty()?alias:prop;
	}
	
	void addAlias(const std::string & prop, const std::string & alias) 
	{
		toAliases.insert(std::make_pair(prop, alias));
		fromAliases.insert(std::make_pair(alias, prop));
	}

	const std::string & getAlias(const std::string & prop) const
	{
		std::string alias;
		auto & it = toAliases.find(prop);
		if (it != toAliases.end())
		{
			alias = (*it).second;
		}
		return alias;
	}

	const std::string & getProperty(const std::string & alias)
	{
		std::string prop;
		auto & it = fromAliases.find(alias);
		if (it != fromAliases.end())
		{
			prop = (*it).second;
		}
		return prop;
	}


private:
	static std::map<std::string, std::shared_ptr<AliasProcessor>> classMap;
	std::map<std::string, std::string> toAliases;
	std::map<std::string, std::string> fromAliases;
};