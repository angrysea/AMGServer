#pragma once
#include <vector>
#include <map>
#include <memory>

const char pathsep = L'.';

class InstanceNode;
class InstanceSpider;

class InstanceTree  
{
public:
	InstanceTree()
	{
	}

	InstanceTree(const InstanceTree & aCopy) = delete;

	InstanceTree(const InstanceTree && aCopy) = delete;

	~InstanceTree();

	int size() const
	{
	   return items.size();
	}

	void insert(const std::string & sTag, std::shared_ptr<InstanceNode> & node);
	bool remove(const std::string & sTag);
	bool remove(const std::shared_ptr<InstanceNode> & pNode);
	const std::shared_ptr<InstanceNode> & getNode(const std::string & sTag) const;
	void getNodes(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList) const;
	const std::shared_ptr<InstanceNode> & findNode(const std::string & sTag) const;
	void findNodes(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList) const;
	void process(std::shared_ptr<InstanceSpider> & spider, std::shared_ptr<InstanceNode> & parent, int levels = 999999999);
	void RenamePath(const std::string & strOldName, const std::string & strNewName);
	void flatten(std::vector<std::shared_ptr<InstanceNode>> & theList) const;
	void getLeaves(std::vector<std::shared_ptr<InstanceNode>> & theList);
	int getRoute(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList);
	int getPath(const std::string & sTag, std::string & sPath) const;
	int getRoutes(const std::string & sTag, std::vector<std::vector<std::shared_ptr<InstanceNode>>> & theList);
	void clear();

	std::multimap<std::string, std::shared_ptr<InstanceNode>>::iterator begin()
	{
	   return items.begin();
	}

	std::multimap<std::string, std::shared_ptr<InstanceNode>>::iterator end()
	{
	   return items.end();
	}

private:
	std::multimap<std::string, std::shared_ptr<InstanceNode>> items;
};
