#include "PeerTree.h"

PeerTree::PeerTree()
{
}

PeerTree::PeerTree(const PeerTree & node) :
nodes{ node.nodes },
parent{ node.parent }
{
}

PeerTree::PeerTree(const PeerTree && node) :
nodes{ std::move(node.nodes) },
parent{ std::move(node.parent) }
{
}

void PeerTree::setParent(PeerNode * node)
{
	parent = node;
}

std::shared_ptr<PeerNode> PeerTree::insert(const PeerEntry & peerEntry)
{
	std::shared_ptr<PeerNode> node(new PeerNode{ peerEntry });
	nodes[node->getAddressPtr()] = node;
	return node;
}

std::shared_ptr<PeerNode> PeerTree::insert(std::shared_ptr<PeerNode> node)
{
	nodes[node->getAddressPtr()] = node;
	return node;
}

std::shared_ptr<const PeerNode> PeerTree::getConnectionNode(int inc, std::shared_ptr<const PeerNode> link) const
{
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (*link != *node)
		{
			if (node->getConnectionNode(inc, std::shared_ptr<PeerNode>{ parent }))
			{
				return std::shared_ptr<const PeerNode>{ node };
			}
		}
	}
	return std::shared_ptr<const PeerNode>();
}

void PeerTree::computeAverage(std::shared_ptr<const PeerNode> link)
{
	PeerNode::average = (PeerNode::average + nodes.size()) / 2;
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (*link == *node)
		{
			node->computeAverage(std::shared_ptr<const PeerNode> { parent });
		}
	}
}

void PeerTree::balance(std::shared_ptr<const PeerNode> link)
{
	if (nodes.size() > PeerNode::average)
	{
		// TODO: balance
	}

	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (*link == *node)
		{
			node->balance(std::shared_ptr<const PeerNode> { parent });
		}
	}
}

void PeerTree::optimize(std::shared_ptr<const PeerNode> link)
{
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (*link == *node)
		{
			node->optimize(std::shared_ptr<const PeerNode>{ parent });
		}
	}
}

void PeerTree::setAlive(std::shared_ptr<const PeerNode> link, bool bAlive)
{
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (*link == *node)
		{
			node->setAlive(std::shared_ptr<PeerNode>{ parent }, bAlive);
		}
	}
}

unsigned int PeerTree::size()
{
	return nodes.size();
}

unsigned int PeerTree::count(std::shared_ptr<const PeerNode> link) const
{
	long l = nodes.size();
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (*link == *node)
		{
			node->count(std::shared_ptr<PeerNode>{ parent });
		}
	}
	return l;
}

std::shared_ptr<PeerNode> PeerTree::operator[](const Address & tag)
{
	return nodes[&tag];
}

std::shared_ptr<const PeerNode> PeerTree::find(const std::string & tag, std::shared_ptr<const PeerNode> link)
{
	return find(Address{ tag }, link);
}

std::shared_ptr<const PeerNode> PeerTree::find(const Address & tag, std::shared_ptr<const PeerNode> link)
{
	std::shared_ptr<PeerNode> & value = nodes[&tag];
	if (value.use_count() == 0 || value == nullptr)
	{
		for (auto & p : nodes)
		{
			auto & child = p.second;
			if (*link == *child)
			{
				std::shared_ptr<const PeerNode> & node = child->find(tag, std::shared_ptr<const PeerNode>{parent});
				if (node.use_count() > 0)
				{
					return node;
				}
			}
		}
	}
	return std::shared_ptr<const PeerNode>();
}

bool PeerTree::remove(const std::string & tag, std::shared_ptr<const PeerNode> link)
{
	return remove(Address{ tag }, link);
}

bool PeerTree::remove(const Address & address, std::shared_ptr<const PeerNode> link)
{
	if (nodes.erase(&address) > 0)
		return true;

	for (auto & p : nodes)
	{
		auto & child = p.second;
		if (*link == *child)
		{
			if (child->remove(address, std::shared_ptr<PeerNode>{ parent }) == true)
				return true;
		}
	}
	return false;
}

void PeerTree::write(std::string & buffer) const
{
	write(buffer, std::shared_ptr<const PeerNode>{nullptr});
}

void PeerTree::write(std::string & buffer, std::shared_ptr<const PeerNode> link) const
{
	int count = 0;
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (link != nullptr && *link != *node)
		{
			count++;
			break;
		}
	}

	if (count)
	{
		for (auto & p : nodes)
		{
			auto & node = p.second;
			if (link != nullptr && *link != *node)
			{
				node->write(buffer, std::shared_ptr<const PeerNode>{parent});
			}
		}
	}
}

void PeerTree::display(std::shared_ptr<const PeerNode> link) const {

	int count = 0;
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (link != nullptr && *link != *node)
		{
			count++;
			break;
		}
	}

	if (count)
	{
		for (auto & p : nodes)
		{
			auto & node = p.second;
			if (link != nullptr && *link != *node)
			{
				node->display(std::shared_ptr<const PeerNode>{parent});
			}
		}
	}
}

void PeerTree::clear(std::shared_ptr<const PeerNode> link)
{
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (link != nullptr && *link != *node)
		{
			node->clear(std::shared_ptr<const PeerNode>{parent});
		}
	}
	nodes.clear();
}

void PeerTree::flatten(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (link != nullptr && *link != *node)
		{
			node->flatten(list, std::shared_ptr<const PeerNode>{parent});
		}
	}
}

bool PeerTree::disconnected(std::shared_ptr<PeerNode> node)
{
	if (node != nullptr)
	{
		if (nodes.erase(node->getAddressPtr()) > 0)
			return true;
	}
	return false;
}

void PeerTree::getLeaves(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	for (auto & p : nodes)
	{
		auto & node = p.second;
		if (link != nullptr && *link != *node)
		{
			if (node->isLeaf())
			{
				list.push_back(node);
				continue;
			}
			node->getLeaves(list, std::shared_ptr<const PeerNode>{ parent });
		}
	}
}

int PeerTree::getRoute(const std::string & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	return getRoute(Address{ tag }, list, link);
}

int PeerTree::getRoute(const Address & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	int nRet = 0;

	auto it = nodes.find(&tag);
	if (it != nodes.end())
	{
		auto & node = it->second;
		if (node != nullptr)
		{
			list.push_back(node);
			nRet = 1;
		}
		else
		{
			for (auto & p : nodes)
			{
				auto & child = p.second;
				if (link != nullptr && *link != *child)
				{
					if ((nRet = child->getRoute(tag, list, std::shared_ptr<const PeerNode>{ parent })) != 0)
					{
						nRet++;
						list.push_back(child);
						break;
					}
				}
			}
		}
	}

	return nRet;
}

int PeerTree::getPath(const std::string & tag, std::string & sPath, std::shared_ptr<const PeerNode> link) const
{
	return getPath(Address{ tag }, sPath, link);
}

int PeerTree::getPath(const Address & address, std::string & sPath, std::shared_ptr<const PeerNode> link) const
{
	int nRet = 0;
	auto it = nodes.find(&address);
	if (it != nodes.end())
	{
		auto & node = it->second;
		if (node != nullptr)
		{
			sPath += address.getURI();
			nRet = 1;
		}
		else
		{
			for (auto & p : nodes)
			{
				auto & child = p.second;
				if (link != nullptr && *link != *child)
				{
					if ((nRet = child->getPath(address, sPath, std::shared_ptr<const PeerNode>{parent})) != 0)
					{
						nRet++;
						sPath += child->getAddress().tostring();
						sPath += ";";
						break;
					}
				}
			}
		}
	}
	return nRet;
}

void PeerTree::getNodes(std::vector<std::shared_ptr<const PeerNode>> & list) const
{
	for (auto & p : nodes)
	{
		list.push_back(p.second);
	}
}