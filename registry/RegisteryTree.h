#pragma once

#include "RegisteryNode.h"

class RegisterySpider;

const char fslash = '\\';

class RegisteryTree  
{
public:
	RegisteryTree();
	~RegisteryTree();
	void insert(RegisteryNode * pNode);
	void insert(const std::string & sTag, RegisteryNode * pNode);
	bool remove(const std::string & sTag);
	bool remove(RegisteryNode * pNode);
	int size() const;
	RegisteryNode * getNode(const std::string & sTag);
	void getNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList);
	RegisteryNode * findNode(const std::string & sTag);
	void findNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList);
	void RenamePath(std::string strOldName, std::string strNewName);
	void flatten(std::vector<RegisteryNode * > & theList) const;
	void getLeaves(std::vector<RegisteryNode * > & theList);
	int getRoute(const std::string & sTag, std::vector<RegisteryNode * > & theList);
	int getPath(const std::string & sTag, std::string & sPath) const;
	int getRoutes(const std::string & sTag, std::vector<std::vector<RegisteryNode * > * > & theList);
	RegisteryNodesIterator begin();
	RegisteryNodesIterator end();
	void clear();
private:
	RegisteryNodes nodes;
	RegisteryNodeVector nodevec;
};
