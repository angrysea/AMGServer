#include <iostream> 
#include <algorithm> 

#include "PeerNode.h"
#include "PeerTree.h"

#include "../messaging/Messenger.h"

unsigned int PeerNode::levels = 5;
Messenger PeerNode::messenger;
unsigned int PeerNode::min = 5;
unsigned int PeerNode::max = 10;
unsigned int PeerNode::average = 0;
short PeerNode::RIGHT = 1;
short PeerNode::LEFT = -1;

PeerNode::PeerNode()
{
	tree = std::make_unique<PeerTree>(PeerTree{});
	tree->setParent(this);
}

PeerNode::PeerNode(const PeerEntry & pe)
{
	peerEntry = pe;
}

std::shared_ptr<const PeerNode> PeerNode::getConnectionNode(int inc, const std::shared_ptr<PeerNode> link) const
{
	if (isAlive())
	{
		unsigned int s = count(link);
		if (s < min || s < (average + inc))
		{
			return std::shared_ptr<const PeerNode>(this);
		}
	}
	return tree->getConnectionNode(inc, link);
}

/*
* Insert a entry in the current tree
*/
std::shared_ptr<PeerNode> PeerNode::insert(const PeerEntry & entry)
{
	auto node = tree->insert(entry);
	// Insert this node into the insert node reverse spin
	node->link(std::shared_ptr<PeerNode>(this));

	return node;
}

/*
* Insert a node in the current tree
*/
std::shared_ptr<PeerNode> PeerNode::insert(std::shared_ptr<PeerNode> node)
{
	// Insert into current tree.
	tree->insert(node);
	// Insert this node into the insert node reverse spin
	node->link(std::shared_ptr<PeerNode>(this));
	return node;
}

/*
* Link a node in the current tree
*/
void PeerNode::link(std::shared_ptr<PeerNode> node)
{
	// Insert into current tree.
	tree->insert(node);
}

// Find a local node
std::shared_ptr<const PeerNode> PeerNode::operator[](const std::string & tag) const
{
	return (*tree)[Address{ tag }];
}

std::shared_ptr<const PeerNode> PeerNode::operator[](const Address & tag) const
{
	return (*tree)[tag];
}

// Find a node through out the known network
std::shared_ptr<const PeerNode> PeerNode::find(const std::string & tag) const
{
	return find(Address{ tag });
}

std::shared_ptr<const PeerNode> PeerNode::find(const Address & tag) const
{
	if (tag == peerEntry.getAddress())
		return std::shared_ptr<const PeerNode>{ this };
	return tree->find(tag, std::shared_ptr<const PeerNode>{ this });
}

// Find a node through out the known network
std::shared_ptr<const PeerNode> PeerNode::find(const std::string & tag, std::shared_ptr<const PeerNode> link) const
{
	return tree->find(Address{ tag }, link);
}

std::shared_ptr<const PeerNode> PeerNode::find(const Address & tag, std::shared_ptr<const PeerNode> link) const
{
	return tree->find(tag, link);
}

bool PeerNode::remove(const PeerEntry & entry)
{
	return tree->remove(entry.getAddress(), std::shared_ptr<PeerNode>{ this });
}

bool PeerNode::remove(const Address & tag)
{
	return tree->remove(tag, std::shared_ptr<const PeerNode>{ this });
}

bool PeerNode::remove(const std::string & tag)
{
	return tree->remove(tag, std::shared_ptr<const PeerNode>{ this });
}

bool PeerNode::remove(const PeerEntry & entry, std::shared_ptr<const PeerNode> link)
{
	return tree->remove(entry.getAddress(), std::shared_ptr<const PeerNode>{ link });
}

bool PeerNode::remove(const Address & tag, std::shared_ptr<const PeerNode> link)
{
	return tree->remove(tag, link);
}

bool PeerNode::remove(const std::string & tag, std::shared_ptr<const PeerNode> link)
{
	return tree->remove(tag, link);
}

int PeerNode::getRoute(const std::string & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	return tree->getRoute(Address{ tag }, list, link);
}

int PeerNode::getRoute(const Address & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	return tree->getRoute(tag, list, link);
}

int PeerNode::getPath(const std::string & tag, std::string & sPath, std::shared_ptr<const PeerNode> link) const
{
	return tree->getPath(tag, sPath, link);
}

int PeerNode::getPath(const Address & tag, std::string & sPath, std::shared_ptr<const PeerNode> link) const
{
	return tree->getPath(tag, sPath, link);
}

void PeerNode::getLeaves(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	tree->getLeaves(list, link);
}

void PeerNode::clear(std::shared_ptr<const PeerNode> link)
{
	tree->clear(link);
}

void PeerNode::computeAverage(std::shared_ptr<const PeerNode> link) const
{
	tree->computeAverage(link);
}

void PeerNode::notifyInserted(const std::string & address) const
{
	std::vector<std::shared_ptr<const PeerNode>> list;
	tree->flatten(list, std::shared_ptr<const PeerNode>{ this });

	for (auto & node : list)
	{
		if (node->isAlive())
		{
			node->sendNotifyInserted(getEntry().getURL(), address);
		}
	}
}

void PeerNode::doMaintenance() const
{
	//MaintenanceController.doPing(peerEntry.getAddress());
}

void PeerNode::doCache() const
{
	//CacheController.doCacheCheck(peerEntry.getAddress());
}

void PeerNode::sendNotifyInserted(const std::string & from, const std::string & to) const
{
	postMessage("dbsws", "insert", from, to);
}

void PeerNode::notifyRemoved(const std::string & from, const std::string & to)
{
	postMessage("dbsws", "remove", from, to);
}

void PeerNode::optimize(std::shared_ptr<const PeerNode> link)
{
	tree->optimize(std::shared_ptr<const PeerNode>{ link });
}

void PeerNode::balance(std::shared_ptr<const PeerNode> link)
{
	tree->balance(std::shared_ptr<const PeerNode>{ link });
}

unsigned int PeerNode::count(std::shared_ptr<const PeerNode> link) const
{
	return tree->count(std::shared_ptr<const PeerNode>{ link });
}

void PeerNode::flatten(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const
{
	tree->flatten(list, std::shared_ptr<const PeerNode>{ link });
}

void PeerNode::getAdjacent(std::vector<std::shared_ptr<const PeerNode>> & list) const
{
	tree->getNodes(list);
	std::remove_if(
		list.begin(),
		list.end(),
		[this](std::shared_ptr<const PeerNode> element) -> bool {
		return *this == *element;
	});
}

bool PeerNode::isLeaf() const
{
	return tree->size() > 0;
}

unsigned int PeerNode::size() const
{
	return tree->size();
}

const PeerEntry & PeerNode::getEntry() const
{
	return peerEntry;
}

const Address & PeerNode::getAddress() const
{
	return peerEntry.getAddress();
}

const Address * PeerNode::getAddressPtr() const
{
	return peerEntry.getAddressPtr();
}

const std::string PeerNode::getURL() const
{
	return peerEntry.getURL();
}

void PeerNode::write(std::string & buffer, std::shared_ptr<const PeerNode> link) const
{
	buffer.append("<Peer>");
	buffer.append("<URI>");
	buffer.append(peerEntry.getURL());
	buffer.append("</URI>");
	buffer.append("<Name>");
	buffer.append(peerEntry.getName());
	buffer.append("</Name>");
	buffer.append("<Time>");
	buffer.append(std::to_string(peerEntry.getTime()));
	buffer.append("</Time>");
	tree->write(buffer, link);
	buffer.append("</Peer>");
}

std::string PeerNode::toString() const
{
	std::string buffer;
	buffer.append("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
	buffer.append("<Peers>");
	write(buffer, std::shared_ptr<const PeerNode>{ this });
	buffer.append("</Peers>");
	return buffer;
}

void PeerNode::toScreen() const
{
	tree->display(std::shared_ptr<const PeerNode>{ this });
}

void PeerNode::display(std::shared_ptr<const PeerNode> link) const
{
	try {
		std::cout << "<Peer>";
		std::cout << "<URI>";
		std::cout << peerEntry.getNameURL();
		std::cout << "</URI>";
		std::cout << "<Name>";
		std::cout << peerEntry.getName();
		std::cout << "</Name>";
		std::cout << "<Time>";
		std::cout << peerEntry.getTime();
		std::cout << "</Time>";
		tree->display(link);
		std::cout << "</Peer>";
	}
	catch (std::exception e) {
	}
}

void PeerNode::setLevels(int levels)
{
	PeerNode::levels = levels;
}

void PeerNode::setMax(int max)
{
	PeerNode::max = max;
	min = max / 2;
}

void PeerNode::setEntry(const PeerEntry & newValue)
{
	peerEntry = newValue;
}

bool PeerNode::isAlive() const
{
	return peerEntry.isAlive();
}

void PeerNode::setAlive(std::shared_ptr<PeerNode> link, bool bAlive)
{
	if (*link != *this)
	{
		peerEntry.setAlive(bAlive);
		tree->setAlive(link, bAlive);
	}
}

int PeerNode::getKey() const
{
	return peerEntry.getKey();
}

bool PeerNode::keyEquals(int value) const
{
	return value == peerEntry.getKey();
}

bool PeerNode::operator==(const PeerNode & peer) const
{
	return peerEntry.getAddress().hashCode() == peer.getEntry()
		.getAddress().hashCode();
}
