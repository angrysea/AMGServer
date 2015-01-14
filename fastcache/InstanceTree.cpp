#include "InstanceNode.h"
#include "InstanceTree.h"
#include "InstanceSpider.h"

InstanceTree::~InstanceTree()
{
	clear();
}

void InstanceTree::insert(const std::string & sTag, std::shared_ptr<InstanceNode> & node)
{
	int start = 0, end = 0;

	if((start = sTag.find(pathsep, 0)) == std::string::npos)
	{
		node->setName(sTag);
		items.insert(std::make_pair(sTag, node));
	}
	else
	{
		std::string name = sTag.substr(0, start);
		auto it = items.find(name);
		if(it != items.end())
		{
			std::string name = sTag.substr(++start);
			(*it).second->insert(name, node);
		}
		else
		{
			std::shared_ptr<InstanceNode> parentNode(new InstanceNode);
			parentNode->setName(name);
			items.insert(std::make_pair(name, parentNode));
			parentNode->insert(sTag.substr(++start), node);
		}
	}
}

bool InstanceTree::remove(const std::string & sTag)
{
	int start = 0, end = 0;

	if((start=sTag.find(pathsep,0))==std::string::npos)
	{
		auto it = items.find(sTag);
		if(it != items.end())
		{
			items.erase(it);
			return true;
		}
		else
		{
			throw RET_FALSE;
		}
	}
	else
	{
		auto it = items.find(sTag.substr(0, start));
		if(it != items.end())
		{
			return (*it).second->remove(sTag.substr(++start));
		}
	}
	return false;
}

bool InstanceTree::remove(const std::shared_ptr<InstanceNode> & node)
{
	auto it = items.find(node->getName());
	if(it!=items.end())
	{
		items.erase(it);
		return true;
	}
	return false;
}

const std::shared_ptr<InstanceNode> & InstanceTree::getNode(const std::string & sTag) const
{
	int start = 0, end = 0;
	if((start = sTag.find(pathsep, 0)) == std::string::npos)
	{
		auto it = items.find(sTag);
		if(it != items.end())
		{
			return (*it).second;
		}
		else
		{
			throw RET_FALSE;
		}
	}
	else
	{
		auto it = items.find(sTag.substr(0, start));
		if(it != items.end())
		{
			return (*it).second->getNode(sTag.substr(++start));
		}
	}

	return nullptr;
}

void InstanceTree::getNodes(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList) const
{
	int start = sTag.find(pathsep, 0);

	if(start==std::string::npos)
	{
		for(auto & p : items)
		{
			auto node = p.second;
			auto it2 = node->begin();
			while(it2!=node->end()) 
			{
				theList.push_back((*it2).second);
				it2++;
			}
		}
	}
	else 
	{
		auto it = items.find(sTag.substr(0, start));
		while(it!=items.end())
		{
			(*it).second->getNodes(sTag.substr(++start),theList);
			it++;
		}
	}
}

const std::shared_ptr<InstanceNode> & InstanceTree::findNode(const std::string & sTag) const
{
	auto it = items.find(sTag);
	if(it != items.end())
	{
		return (*it).second;
	}
	else
	{
		it = items.begin();
		while(it != items.end())
		{
			const std::shared_ptr<InstanceNode> & node = (*it).second->findNode(sTag);
			if(node!=nullptr)
			{
				return node;
			}
			it++;
		}
	}
	return nullptr;
}

void InstanceTree::findNodes(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList) const
{
	auto it = items.find(sTag);
	while(it != items.end())
	{
		theList.push_back((*it).second);
	}

	it = items.begin();
	while(it!=items.end())
	{
		(*it).second->findNodes(sTag,theList);
		it++;
	}
}

void InstanceTree::process(std::shared_ptr<InstanceSpider> & spider, std::shared_ptr<InstanceNode> & parent, int levels)
{
	if(levels!=999999999)
		levels--;

	auto it = items.begin();
	while(it!=items.end())
	{
		auto item = (*it).second;
		spider->nextNode(item,parent);
		if(levels>0)
			item->process(spider,item,levels);
		it++;
	}
}

void InstanceTree::RenamePath(const std::string & strOldName, const std::string & strNewName)
{
	auto node = findNode(strOldName);
	if (node != nullptr)
	{
		items.erase(strOldName);
		items.insert(std::make_pair(strNewName, node));
	}
}

void InstanceTree::flatten(std::vector<std::shared_ptr<InstanceNode>> & theList) const
{
	for(auto & pair : items)
	{
		auto child = pair.second;
		theList.push_back(child);
		child->flatten(theList);
	}
}


void InstanceTree::getLeaves(std::vector<std::shared_ptr<InstanceNode>> & theList)
{
	for(auto & pair : items)
	{
		auto child = pair.second;

		if(child->isLeaf())
		{
			theList.push_back(child);
			continue;
		}
		child->getLeaves(theList);
	}
}

int InstanceTree::getRoute(const std::string & sTag, std::vector<std::shared_ptr<InstanceNode>> & theList)
{
	int nRet = 0;

	auto it = items.find(sTag);
	if(it != items.end())
	{
		theList.push_back((*it).second);
		nRet = 1;
	}
	else
	{
		it = items.begin();
		while(it != items.end())
		{
		if((nRet =(*it).second->getRoute(sTag, theList)) != 0)
		{
			nRet++;
			theList.push_back((*it).second);
			break;
		}
		it++;
		}
	}

	return 0;
}

int InstanceTree::getPath(const std::string & sTag, std::string & sPath) const
{
	int nRet = 0;

	auto it = items.find(sTag);
	if(it != items.end())
	{
		sPath +=(*it).first;
		nRet = 1;
	}
	else
	{
		it = items.begin();
		while(it != items.end())
		{
			if((nRet =(*it).second->getPath(sTag, sPath)) != 0)
			{
				nRet++;
				sPath +=(*it).first;
				sPath += pathsep;
				break;
			}
			it++;
		}
	}
	return 0;
}

int InstanceTree::getRoutes(const std::string & sTag, std::vector< std::vector<std::shared_ptr<InstanceNode>> > & theList)
{
	int nRet = 0;

	auto it = items.find(sTag);
	if(it != items.end())
	{
		auto & node = (*it).second;
		std::vector<std::shared_ptr<InstanceNode>> list;
		list.push_back(node);
		theList.push_back(list);
		nRet = 1;
	}
	else
	{
		it = items.begin();
		while(it != items.end())
		{
			std::vector<std::vector<std::shared_ptr<InstanceNode>>> myList;
			if((nRet =(*it).second->getRoutes(sTag, myList)) != 0)
			{
				auto qit = myList.begin();
				while(qit != myList.end())
				{
					auto & list = (*qit);
					list.push_back((*it).second);
					theList.push_back(list);
					qit++;
				}
				nRet++;
				break;
			}
			it++;
		}
	}

	return 0;
}

void InstanceTree::clear()
{
	items.clear();
}

