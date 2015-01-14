#pragma once

#include <string>
#include <vector>
#include <exception>

class Body 
{
public:

	Body() 
	{
	}

	const std::string & get(int i) const
	{
		return _content.at(i);
	}

	void add(std::string newValue) 
	{
		_content.push_back(newValue);
	}

	int getcontentSize() 
	{
		return _content.size();
	}

private:
	std::vector<std::string> _content;
};
