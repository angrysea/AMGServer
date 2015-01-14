#pragma once

#pragma warning(disable:4786)

#include <string>
#include <stack>
#include <map>
#include <vector>

class RegisteryTree;
class RegisterySpider;

class RegisteryNode
{
public:
	enum Etype { NONE, KEY, VALUE };

	RegisteryNode(RegisteryNode* pParent, Etype t = KEY)
		:m_pTree(0),
		 type(t),
       m_pParent(pParent)
	{
	}

	RegisteryNode(RegisteryNode* pParent, const std::string & n, Etype t = KEY)
		:m_pTree(0),
		 name(n),
		 type(t),
       m_pParent(pParent)
	{
	}

	RegisteryNode(RegisteryNode* pParent, const std::string & n, const std::string & v, Etype t = KEY)
		:m_pTree(0),
		 name(n),
		 value(v),
		 type(t),
       m_pParent(pParent)
	{
	}

	RegisteryNode(RegisteryNode* pParent, const std::string & n, const std::string & v, const std::string & vt, Etype t = KEY)
		:m_pTree(0),
		 name(n),
		 value(v),
		 valueType(vt),
		 type(t),
       m_pParent(pParent)
	{
	}

	~RegisteryNode();

	RegisteryNode* getParent()
	{
		return m_pParent;
	}

	bool isRoot()
	{
		return m_pParent==0 ? true : false;
	}

	void checkMap();
	void insert(RegisteryNode * pNode);
	void insert(const std::string & sTag, RegisteryNode * pNode);
	bool remove(const std::string & sTag);
	RegisteryNode * getNode(const std::string & sTag);
	void getNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList);
	RegisteryNode * findNode(const std::string & sTag);
	void findNodes(const std::string & sTag, std::vector<RegisteryNode * > & theList);
	void RenamePath(std::string strOldName, std::string strNewName);
	int getRoute(const std::string & sTag, std::vector<RegisteryNode * > & theList);
	int getPath(const std::string & sTag, std::string & sPath) const;
	int getRoutes(const std::string & sTag, std::vector<std::vector<RegisteryNode * > * > & theList);
	void getLeaves(std::vector<RegisteryNode * > & theList);
	void flatten(std::vector<RegisteryNode * > & theList) const;
	std::multimap<std::string,RegisteryNode * >::iterator begin();
	std::multimap<std::string,RegisteryNode * >::iterator end();
	bool isLeaf() const;

	std::string & getName()
	{
		return name;
	}

	std::string & getValue()
	{
		return value;
	}

	std::string & getValueType()
	{
		return valueType;
	}

	void setData(char * buf, int offset, int len)
	{
		value += std::string(buf+offset,len);
	}
	
	Etype getType() const
	{
		return type;
	}

private:	
	RegisteryNode * m_pParent;
	RegisteryTree * m_pTree;
	Etype type;
	std::string name;
	std::string valueType;
	std::string value;
};

typedef std::stack<RegisteryNode * > RegisteryNodeStack;
typedef std::vector<RegisteryNode * > RegisteryNodeVector;
typedef std::vector<RegisteryNode * >::iterator RegisteryNodeVectorIterator;
typedef std::multimap< std::string, RegisteryNode * > RegisteryNodes;
typedef std::multimap<std::string,RegisteryNode * >::iterator RegisteryNodesIterator;
typedef std::multimap<std::string,RegisteryNode * >::const_iterator RegisteryNodesConstIterator;
typedef std::multimap<std::string,RegisteryNode * >::value_type RegisteryNodesValueType;


