#include "RegisteryTree.h"
#include "RegisterySpider.h"

RegisteryTree::RegisteryTree()
{
}

RegisteryTree::~RegisteryTree()
{
	for(RegisteryNodesConstIterator it = nodes.begin(); it != nodes.end(); it++)
	{
	  RegisteryNode * child =(*it).second;
	  delete child;
	}
}

void RegisteryTree::insert(RegisteryNode * pNode)
{
	nodes.insert(RegisteryNodesValueType(pNode->getName(), pNode));
	nodevec.push_back(pNode);
}

void RegisteryTree::insert(const std::string & sTag, RegisteryNode * pNode)
{
	int start = 0, end = 0;

	if((start = sTag.find(fslash, 0)) == std::string::npos)
	{
	  nodes.insert(RegisteryNodesValueType(sTag, pNode));
	  nodevec.push_back(pNode);
	}
	else
	{
	  RegisteryNodesIterator it;
	  it = nodes.find(sTag.substr(0, start));
	  if(it != nodes.end())
	  {
		(*it).second->insert(sTag.substr(++start), pNode);
	  }
	}
}

bool RegisteryTree::remove(const std::string & sTag)
{
	RegisteryNode * pNode = 0;
	int start = 0, end = 0;

	try
	{
		RegisteryNodesIterator it;
		if((start=sTag.find(fslash,0))==std::string::npos)
		{
			it = nodes.find(sTag);
			if(it != nodes.end())
			{
				pNode =(*it).second;
				nodes.erase(it);
				RegisteryNodeVectorIterator itv = nodevec.begin();
				while(itv!=nodevec.end())
				{
					if(pNode==(*itv))
					{
						nodevec.erase(itv);
						break;
					}
				}
				return true;
			}
			else
			{
				throw false;
			}
		}
		else
		{
			it = nodes.find(sTag.substr(0, start));
			if(it != nodes.end())
			{
				return (*it).second->remove(sTag.substr(++start));
			}
		}
	}
	catch(bool)
	{
	}
	return false;
}

bool RegisteryTree::remove(RegisteryNode * pNode)
{
	RegisteryNode * pTemp = 0;

	try
	{
		RegisteryNodesIterator it;

		it = nodes.find(pNode->getName());
		if(it!=nodes.end())
		{
			pTemp =(*it).second;
			nodes.erase(it);
			RegisteryNodeVectorIterator itv = nodevec.begin();
			while(itv!=nodevec.end())
			{
				if(pTemp==(*itv))
				{
					nodevec.erase(itv);
					break;
				}
			}
			return true;
		}
	}
	catch(bool)
	{
	}
	return false;
}

int RegisteryTree::size() const
{
	return nodes.size();
}

RegisteryNode * RegisteryTree::getNode(const std::string & sTag)
{
	int start = 0, end = 0;
	RegisteryNode * pNode = 0;

	try
	{
	  RegisteryNodesIterator it;
	  if((start = sTag.find(fslash, 0)) == std::string::npos)
	  {
		 it = nodes.find(sTag);
		 if(it != nodes.end())
		 {
			pNode =(*it).second;
		 }
		 else
		 {
			throw false;
		 }
	  }
	  else
	  {
		 it = nodes.find(sTag.substr(0, start));
		 if(it != nodes.end())
		 {
			pNode =(*it).second->getNode(sTag.substr(++start));
		 }
	  }
	}
	catch(bool)
	{
	}

	return pNode;
}

void RegisteryTree::getNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList)
{
	int start = 0, end = 0;

	try
	{
		RegisteryNodesIterator it;
		if((start=sTag.find(fslash, 0)) == std::string::npos)
		{
			
			for( it = nodes.find(sTag); it!=nodes.end(); it++ )
			{
				theList.push_back((*it).second);
			}
		}
/*		
		for( it = nodes.find(sTag.substr(0, start)); it!=nodes.end(); it ++)
		{
			(*it).second->getNodes(sTag.substr(++start),theList);
		}
*/
	}
	catch(bool)
	{
	}
}

RegisteryNode * RegisteryTree::findNode(const std::string & sTag)
{
	RegisteryNode * pNode = 0;

	try
	{
	  RegisteryNodesIterator it;

	  it = nodes.find(sTag);
	  if(it != nodes.end())
	  {
		 pNode =(*it).second;
	  }
	  else
	  {
		 it = nodes.begin();
		 while(it != nodes.end())
		 {
			if((pNode =(*it).second->findNode(sTag)) != 0)
			{
				break;
			}
			it++;
		 }
	  }
	}
	catch(bool)
	{
	}

	return pNode;
}

void RegisteryTree::findNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList)
{
	try
	{
		RegisteryNodesIterator it;

		it = nodes.find(sTag);
		while(it != nodes.end())
		{
			theList.push_back((*it).second);
		}

		it = nodes.begin();
		while(it!=nodes.end())
		{
			(*it).second->findNodes(sTag,theList);
			it++;
		}
	}
	catch(bool)
	{
	}

}

void RegisteryTree::RenamePath(std::string strOldName, std::string strNewName)
{
	RegisteryNode* pNode = findNode(strOldName);
	if (pNode != 0)
	{
	  nodes.erase(strOldName);
	  nodes.insert(RegisteryNodesValueType(strNewName, pNode));
	}
}

void RegisteryTree::flatten(std::vector<RegisteryNode * > & theList) const
{
	for(RegisteryNodesConstIterator it = nodes.begin(); it != nodes.end(); it++)
	{
	  RegisteryNode * child =(*it).second;
	  theList.push_back(child);
	  child->flatten(theList);
	}
}


void RegisteryTree::getLeaves(std::vector<RegisteryNode * > & theList)
{
	for(RegisteryNodesIterator it = nodes.begin(); it != nodes.end(); it++)
	{
	  RegisteryNode * child =(*it).second;

	  if(child->isLeaf())
	  {
		 theList.push_back(child);
		 continue;
	  }
	  child->getLeaves(theList);
	}
}

int RegisteryTree::getRoute(const std::string & sTag, std::vector<RegisteryNode * > & theList)
{
	int nRet = 0;
	try
	{
	  RegisteryNodesIterator it;

	  it = nodes.find(sTag);
	  if(it != nodes.end())
	  {
		 theList.push_back((*it).second);
		 nRet = 1;
	  }
	  else
	  {
		 it = nodes.begin();
		 while(it != nodes.end())
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
	}
	catch(...)
	{
	  nRet = 0;
	}

	return 0;
}

int RegisteryTree::getPath(const std::string & sTag, std::string & sPath) const
{
	int nRet = 0;
	try
	{
	  RegisteryNodesConstIterator it = nodes.find(sTag);
	  if(it != nodes.end())
	  {
		 sPath +=(*it).first;
		 nRet = 1;
	  }
	  else
	  {
		 it = nodes.begin();
		 while(it != nodes.end())
		 {
			if((nRet =(*it).second->getPath(sTag, sPath)) != 0)
			{
				nRet++;
				sPath +=(*it).first;
				sPath += ".";
				break;
			}
			it++;
		 }
	  }
	}
	catch(...)
	{
	  nRet = 0;
	}

	return 0;
}

int RegisteryTree::getRoutes(const std::string & sTag, std::vector<std::vector<RegisteryNode * > * > & theList)
{
	int nRet = 0;

	try
	{
		RegisteryNodesIterator it;

		it = nodes.find(sTag);
		if(it != nodes.end())
		{
			std::vector<RegisteryNode * > * pList = new std::vector<RegisteryNode * >;
			pList->push_back((*it).second);
			theList.push_back(pList);
			nRet = 1;
		}
		else
		{
			it = nodes.begin();
			while(it != nodes.end())
			{
				std::vector<std::vector<RegisteryNode * > * > myList;
				if((nRet =(*it).second->getRoutes(sTag, myList)) != 0)
				{
					std::vector<std::vector<RegisteryNode * > * >::iterator qit = myList.begin();
					while(qit != myList.end())
					{
						std::vector<RegisteryNode * > * pList =(*qit);
						pList->push_back((*it).second);
						theList.push_back(pList);
						qit++;
					}
					nRet++;
					break;
				}
				it++;
			}
		}
	}
	catch(...)
	{
		nRet = 1;
	}

	return 0;
}

RegisteryNodesIterator RegisteryTree::begin()
{
	return nodes.begin();
}

RegisteryNodesIterator RegisteryTree::end()
{
	return nodes.end();
}


void RegisteryTree::clear()
{
	try
	{
		nodevec.clear();
		RegisteryNodesIterator it = nodes.begin();
		while(it!=nodes.end())
		{
			delete (*it).second;
		}
		nodes.clear();
	}
	catch(bool)
	{
	}
}

