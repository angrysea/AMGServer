#pragma once

#include <memory>
#include <exception>

#include "PeerEntry.h"
#include "../messaging/Message.h"
#include "../messaging/Messenger.h"

class PeerTree;
class PeerNode
{
public:
	PeerNode();
	PeerNode(const PeerEntry & pe);

	std::shared_ptr<const PeerNode> getConnectionNode(int inc, std::shared_ptr<PeerNode> link) const;
	std::shared_ptr<PeerNode> insert(const PeerEntry & entry);
	std::shared_ptr<PeerNode> insert(std::shared_ptr<PeerNode> node);
	void link(std::shared_ptr<PeerNode> node);
	std::shared_ptr<const PeerNode> operator[](const std::string & tag) const;
	std::shared_ptr<const PeerNode> operator[](const Address & tag) const;
	std::shared_ptr<const PeerNode> find(const std::string & tag) const;
	std::shared_ptr<const PeerNode> find(const Address & tag) const;
	std::shared_ptr<const PeerNode> find(const std::string & tag, std::shared_ptr<const PeerNode> link) const;
	std::shared_ptr<const PeerNode> find(const Address & tag, std::shared_ptr<const PeerNode> link) const;
	bool remove(const PeerEntry & entry);
	bool remove(const Address & tag);
	bool remove(const std::string & tag);
	bool remove(const PeerEntry & entry, std::shared_ptr<const PeerNode> link);
	bool remove(const Address & tag, std::shared_ptr<const PeerNode> link);
	bool remove(const std::string & tag, std::shared_ptr<const PeerNode> link);
	int getRoute(const std::string & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	int getRoute(const Address & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	int getPath(const std::string & tag, std::string & sPath, std::shared_ptr<const PeerNode> link) const;
	int getPath(const Address & tag, std::string & sPath, std::shared_ptr<const PeerNode> link) const;
	void getLeaves(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	void clear(std::shared_ptr<const PeerNode> link);
	void computeAverage(std::shared_ptr<const PeerNode> link) const;
	void notifyInserted(const std::string & address) const;
	void doMaintenance() const;
	void doCache() const;
	void sendNotifyInserted(const std::string & from, const std::string & to) const;
	void notifyRemoved(const std::string & from, const std::string & to);
	void optimize(std::shared_ptr<const PeerNode> link);
	void balance(std::shared_ptr<const PeerNode> link);
	unsigned int count(std::shared_ptr<const PeerNode> link) const;
	void flatten(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	void getAdjacent(std::vector<std::shared_ptr<const PeerNode>> & list) const;
	bool isLeaf() const;
	unsigned int size() const;
	const PeerEntry & getEntry() const;
	const Address & getAddress() const;
	const Address * getAddressPtr() const;
	const std::string getURL() const;
	void write(std::string & buffer, std::shared_ptr<const PeerNode> link) const;
	std::string toString() const;
	void toScreen() const;
	void display(std::shared_ptr<const PeerNode> link) const;

	void setEntry(const PeerEntry & peerEntry);
	bool isAlive() const;
	void setAlive(std::shared_ptr<PeerNode>, bool bAlive);
	int getKey() const;
	bool keyEquals(int value) const;
	bool operator == (const PeerNode & peer) const;
	bool operator != (const PeerNode & peer) const;

	template <typename ...T>
	void broadcastMessage(const std::string & plugin, const std::string & request, T &&... args) const
	{
		Message message(peerEntry.getURL());
		message.getAddress().setPlugin(plugin);
		message.getAddress().setMethod(request);
		tree->broadcastMessage(message, std::forward<T>(args));
	}

	template <typename ...T>
	void PeerNode::broadcastMessage(const Message & message, T &&... args) const
	{
		tree->broadcastMessage(message, std::forward<T>(args));
	}

	template <typename ...T>
	void postMessage(const std::string & plugin, const std::string & request, T &&... args) const
	{
		Message message = new Message(peerEntry.getURL());
		message.getAddress().setPlugin(plugin);
		message.getAddress().setMethod(request);
		tree->broadcastMessage(message, std::forward<T>(args));
	}

	template <typename ...T>
	void postMessage(const Message & message, T &&... args) const
	{
		if (isAlive())
		{
			message.getAddress().setHost(peerEntry.getAddress().getHost());
			message.getAddress().setPort(peerEntry.getAddress().getPort());
			Messenger.postMessage(message, std::forward<T>(args));
		}
	}

	static void setLevels(int levels);
	static void setMax(int max);

public:
	static unsigned int levels;
	static unsigned int min;
	static unsigned int max;
	static unsigned int average;
	static short RIGHT;
	static short LEFT;

protected:
	PeerEntry peerEntry;
	std::unique_ptr<PeerTree> tree;
	static Messenger messenger;
};

