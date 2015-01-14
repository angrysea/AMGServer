#pragma once
#include "RegFile.h"
class RegisteryNode;

class RegisterySpider  
{
public:

	void save(RegisteryNode * node,std::string key, RegFile & regFile, int level=0)
	{
		level++;

		std::string line;
		if(node->getType()==RegisteryNode::VALUE)
		{
			if(node->getName()=="@")
			{
				line.append(node->getName());
				line.append("=");
			}
			else
			{
				line.append("\"");
				line.append(node->getName());
				line.append("\"=");
			}
			if(node->getValueType()=="Text")
			{
				line.append("\"");
				line.append(node->getValue());
				line.append("\"");
			}
			else
			{
				line.append(node->getValueType());
				line.append(":");
				line.append(node->getValue());
			}
			regFile.writeLine(line);
		}
		else
		{
			RegisteryNode * next = 0;
			if(node->isRoot())
			{
				line.append(node->getValue());
				regFile.writeLine(line);
				regFile.blankLine();
				key = "[";
				RegisteryNodesIterator it = node->begin();
				while(it!=node->end())
				{
					next = (*it).second;
					if(next->getType()==RegisteryNode::KEY)
					{
						save(next, key, regFile, level);
					}
					it++;
				}
			}
			else
			{
				key += node->getName();

				if(node->isLeaf())
				{
					line.append(key);
					line.append("]");
					regFile.writeLine(line);
					regFile.blankLine();
				}
				else
				{
					bool bValueNodes = false;
					RegisteryNodesIterator it = node->begin();
					while(it!=node->end())
					{
						next = (*it).second;
						if(next->getType()==RegisteryNode::VALUE)
						{
							bValueNodes=true;
							break;
						}
						it++;
					}
					
					if(bValueNodes)
					{
						line.append(key);
						line.append("]");
						regFile.writeLine(line);
						RegisteryNodesIterator it = node->begin();
						while(it!=node->end())
						{
							next = (*it).second;
							if(next->getType()==RegisteryNode::VALUE)
							{
								save(next, key, regFile, level);
							}
							it++;
						}
						regFile.blankLine();
					}
					else if(level>3)
					{
						line.append(key);
						line.append("]");
						regFile.writeLine(line);
						regFile.blankLine();
					}


					it = node->begin();
					while(it!=node->end())
					{
						next = (*it).second;
						if(next->getType()==RegisteryNode::KEY)
						{
							save(next, key+"\\", regFile, level);
						}
						it++;
					}
				}
			}
		}
	}
};
