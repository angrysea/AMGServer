#pragma once 

#include <string>

class PropData 
{
public:
	PropData(std::string name, std::string id, std::string state) :
		name{ name }, id{ id }, state{ state }
	{
	}

	PropData(const PropData & aCopy) = default;

	PropData(const PropData && aCopy) :
		name{ std::move(aCopy.name) }, 
		id{ std::move(aCopy.id) }, 
		state{ std::move(aCopy.state) }
	{
	}

	PropData & operator = (const PropData & aCopy) = default;

	PropData & operator = (const PropData && aCopy)
	{
		name = std::move(aCopy.name);
		id = std::move(aCopy.id);
		state = std::move(aCopy.state);
	}

	const std::string & getName() const
	{
		return name;
	}

	const std::string & getId() const
	{
		return id;
	}

	const std::string & getState() const
	{
		return state;
	}

private:
	std::string name;
	std::string id;
	std::string state;
};