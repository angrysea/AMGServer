#include "RegisteryNode.h"
#include "RegisteryTree.h"

RegisteryNode::~RegisteryNode()
{
	delete m_pTree;
}

void RegisteryNode::checkMap()
{
   if(m_pTree == 0)
   {
	  m_pTree = new RegisteryTree;
   }
}

void RegisteryNode::insert(RegisteryNode * pNode)
{
   checkMap();
   m_pTree->insert(pNode);
}

void RegisteryNode::insert(const std::string & sTag, RegisteryNode * pNode)
{
   checkMap();
   m_pTree->insert(sTag, pNode);
}

RegisteryNode * RegisteryNode::getNode(const std::string & sTag)
{
   RegisteryNode * pNode = 0;

   if(m_pTree != 0)
   {
	  pNode = m_pTree->getNode(sTag);
   }

   return pNode;
}

void RegisteryNode::getNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList)
{
   if(m_pTree != 0)
   {
	  m_pTree->getNodes(sTag,theList);
   }
}

RegisteryNode * RegisteryNode::findNode(const std::string & sTag)
{
   RegisteryNode * pNode = 0;

   if(m_pTree != 0)
   {
	  pNode = m_pTree->findNode(sTag);
   }

   return pNode;
}

void RegisteryNode::findNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList)
{
   if(m_pTree != 0)
   {
	  m_pTree->findNodes(sTag,theList);
   }
}

bool RegisteryNode::remove(const std::string & sTag)
{
   bool bRet = false;

   if(m_pTree != 0)
   {
	  bRet = m_pTree->remove(sTag);
   }

   return bRet;
}

void RegisteryNode::RenamePath(std::string strOldName, std::string strNewName)
{
   if (m_pTree != 0)
   {
	  m_pTree->RenamePath(strOldName, strNewName);
   }
}

int RegisteryNode::getRoute(const std::string & sTag, std::vector<RegisteryNode * > & theList)
{
   int nRet = 0;
   if(m_pTree != 0)
   {
	  nRet = m_pTree->getRoute(sTag, theList);
   }
   return nRet;
}

int RegisteryNode::getPath(const std::string & sTag, std::string & sPath) const
{
   int nRet = 0;
   if(m_pTree != 0)
   {
	  nRet = m_pTree->getPath(sTag, sPath);
   }
   return nRet;
}

int RegisteryNode::getRoutes(const std::string & sTag, std::vector<std::vector<RegisteryNode * > * > & theList)
{
   int nRet = 0;
   if(m_pTree != 0)
   {
	  nRet = m_pTree->getRoutes(sTag, theList);
   }
   return nRet;
}

void RegisteryNode::getLeaves(std::vector<RegisteryNode * > & theList)
{
   if(m_pTree != 0)
   {
	  m_pTree->getLeaves(theList);
   }
}

void RegisteryNode::flatten(std::vector<RegisteryNode * > & theList) const
{
   if(m_pTree != 0)
   {
	  m_pTree->flatten(theList);
   }
}

RegisteryNodesIterator RegisteryNode::begin()
{
   checkMap();
   return m_pTree->begin();
}

RegisteryNodesIterator RegisteryNode::end()
{
   checkMap();
   return m_pTree->end();
}

bool RegisteryNode::isLeaf() const
{ 
	return m_pTree == 0 ? true : false; 
}
