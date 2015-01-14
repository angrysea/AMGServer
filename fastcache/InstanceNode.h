#pragma once
#include <stack>
#include <vector>
#include <map>
#include <memory>
#include <string>

#include "Item.h"
#include "InstanceTree.h"

class InstanceSpider;

class InstanceNode
{
public:
	InstanceNode()
	{
	}

	InstanceNode(const InstanceNode & aCopy) = delete;
	InstanceNode(const InstanceNode && aCopy) = delete;

	~InstanceNode() = default;

	void insert(const std::string & sTag, std::shared_ptr<InstanceNode> & node)
	{
			tree->insert(sTag, node);
	}

	std::shared_ptr<InstanceNode> getNode(const std::string & sTag)
	{
		if(tree != nullptr)
		{
			return tree->getNode(sTag);
		}
		return nullptr;
	}

	void getNodes(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList)
	{
		if (tree != nullptr)
		{
			tree->getNodes(sTag,theList);
		}
	}

	void process(std::shared_ptr<InstanceSpider> & spider, std::shared_ptr<InstanceNode> & parent, int levels)
	{
		if (tree != nullptr)
		{
			tree->process(spider, parent, levels);
		}
	}

	const std::shared_ptr<InstanceNode> & findNode(const std::string & sTag) const
	{
	   if (tree != nullptr)
	   {
		  return tree->findNode(sTag);
	   }
	   return nullptr;
	}

	void findNodes(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList) const
	{
		tree->findNodes(sTag,theList);
	}

	bool remove(const std::string & sTag)
	{
		bool bRet = false;

		if (tree != nullptr)
		{
			bRet = tree->remove(sTag);
		}
		return bRet;
	}

	void RenamePath(const std::string & strOldName, const std::string & strNewName)
	{
		if (tree != nullptr)
		{
			tree->RenamePath(strOldName, strNewName);
		}
	}

	int getRoute(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList) const
	{
		int nRet = 0;
		if (tree != nullptr)
		{
			nRet = tree->getRoute(sTag, theList);
		}
		return nRet;
	}

	int getPath(const std::string & sTag, std::string & sPath) const
	{
	   int nRet = 0;
	   if (tree != nullptr)
	   {
		  nRet = tree->getPath(sTag, sPath);
	   }
	   return nRet;
	}

	int getRoutes(const std::string & sTag, std::vector<std::vector<std::shared_ptr<InstanceNode>>> & theList)
	{
	   int nRet = 0;
	   if (tree != nullptr)
	   {
		  nRet = tree->getRoutes(sTag, theList);
	   }
	   return nRet;
	}

	void getLeaves(std::vector<std::shared_ptr<InstanceNode>> & theList)
	{
		if (tree != nullptr)
		{
		  tree->getLeaves(theList);
	   }
	}

	void flatten(std::vector<std::shared_ptr<InstanceNode>> & theList) const
	{
		if (tree != nullptr)
		{
		  tree->flatten(theList);
	   }
	}

	std::multimap<std::string, std::shared_ptr<InstanceNode>>::iterator begin()
	{
	   return tree->begin();
	}

	std::multimap<std::string, std::shared_ptr<InstanceNode>>::iterator end()
	{
	   return tree->end();
	}

	bool isLeaf() const
	{ 
		return tree == 0 ? true : false; 
	}

	const std::string getName() const
	{
		return name;
	}

	void setName(std::string newValue)
	{
		name = newValue;
	}

	std::shared_ptr<InstanceNode> & getValue()
	{
		return value;
	}
	
	void setValue(std::shared_ptr<InstanceNode> & newValue)
	{
		value = newValue;
	}
	
private:	
	std::unique_ptr<InstanceTree> tree;
	std::string name;
	std::shared_ptr<InstanceNode> value;
};
