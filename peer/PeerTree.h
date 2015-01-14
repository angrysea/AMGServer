#pragma once

#include <memory>
#include <vector>
#include <map>

#include "PeerNode.h"
#include "../messaging/Message.h"

class PeerTree
{
public:
	PeerTree();
	PeerTree(const PeerTree & node);
	PeerTree(const PeerTree && node);

	void setParent(PeerNode * node);
	std::shared_ptr<PeerNode> insert(const PeerEntry & peerEntry);
	std::shared_ptr<PeerNode> insert(std::shared_ptr<PeerNode> node);
	std::shared_ptr<const PeerNode > getConnectionNode(int inc, std::shared_ptr<const PeerNode> link) const;
	void computeAverage(std::shared_ptr<const PeerNode> link);
	void balance(std::shared_ptr<const PeerNode > link);
	void optimize(std::shared_ptr<const PeerNode > link);
	void setAlive(std::shared_ptr<const PeerNode > link, bool bAlive);
	unsigned int size();
	unsigned int count(const std::shared_ptr<const PeerNode> link) const;
	std::shared_ptr<const PeerNode> get(const Address & tag);
	std::shared_ptr<const PeerNode> find(const std::string & tag, std::shared_ptr<const PeerNode> link);
	std::shared_ptr<const PeerNode> find(const Address & tag, std::shared_ptr<const PeerNode> link);
	bool remove(const std::string & tag, std::shared_ptr<const PeerNode> link);
	bool remove(const Address & address, std::shared_ptr<const PeerNode> link);
	void write(std::string & buffer) const;
	void write(std::string & buffer, std::shared_ptr<const PeerNode> link) const;
	void display(std::shared_ptr<const PeerNode> link) const;
	void clear(std::shared_ptr<const PeerNode> link);
	void flatten(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	bool disconnected(std::shared_ptr<PeerNode> node);
	void getLeaves(std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	int getRoute(const std::string & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	int getRoute(const Address & tag, std::vector<std::shared_ptr<const PeerNode>> & list, std::shared_ptr<const PeerNode> link) const;
	int getPath(const std::string & tag, std::string & sPath, std::shared_ptr<const PeerNode> link) const;
	int getPath(const Address & address, std::string & sPath, std::shared_ptr<const PeerNode> link) const;

	template <typename ...T>
	void broadcastMessage(const std::string & plugin, const std::string & request, T &&... args) const
	{
		Message message(peerEntry.getURL());
		message.getAddress().setPlugin(plugin);
		message.getAddress().setMethod(request);
		broadcastMessage(message, std::forward<T>(args));
	}

	template <typename ...T>
	void broadcastMessage(const Message & message, T &&... args) const
	{
		std::vector<std::shared_ptr<const PeerNode>> list;
		flatten(list, std::shared_ptr<const PeerNode>{ parent });
		Address sender = message.getReplyTo();
		for (auto node : list)
		{
			if (sender!=node->getAddress())
			{
				node->postMessage(message, std::forward<T>(args));
			}
		}
	}

	void getNodes(std::vector<std::shared_ptr<const PeerNode>> & list) const;
	std::shared_ptr<PeerNode> operator[](const Address & tag);

private:
	std::map<const Address *, std::shared_ptr<PeerNode>> nodes;
	PeerNode * parent;
};
