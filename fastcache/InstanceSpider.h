#pragma once

#include <memory>

class Item;
class InstanceNode;

class InstanceSpider  
{
public:
	InstanceSpider()
	{
	}

	virtual ~InstanceSpider()
	{
	}

	virtual void begin()
	{
	}

	virtual void end()
	{
	}

	virtual void hasAttribute(std::shared_ptr<InstanceNode> & instanceItem, std::shared_ptr<Item> & item)
	{
	}

	virtual void nextNode(std::shared_ptr<InstanceNode> & next, std::shared_ptr<InstanceNode> & parent) = 0;
};
