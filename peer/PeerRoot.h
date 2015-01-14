#pragma once
#include <string>
#include <vector>

#include "PeerNode.h"

class PeerRoot : public PeerNode 
{
public:
	PeerRoot(const std::string & host, int port);
	PeerRoot(const PeerEntry & peerEntry);
	PeerRoot();

	static void initRoot();
	std::shared_ptr<const PeerNode> isConnected(const PeerEntry & entry) const;
	std::shared_ptr<const PeerNode> doConnect(const PeerEntry & entry);
	std::shared_ptr<const PeerNode> getConnectionNode() const;
	void maintenance();
	void cache();
	void refuseConnection(const std::string & peer) const;
	void doDisconnect();
	void doReplace(const std::string & address, const std::string & xml);
	void parsePeers(const std::string & bytes);
	void setAlive(bool bAlive);
	unsigned int count() const;
	int getRoute(const std::string & tag, std::vector<std::shared_ptr<const PeerNode>> & list) const;
	int getRoute(const Address & tag, std::vector<std::shared_ptr<const PeerNode>> & list) const;
	int getPath(const std::string & tag, std::string & sPath) const;
	int getPath(const Address & tag, std::string & sPath) const;
	void getLeaves(std::vector<std::shared_ptr<const PeerNode>> & list) const;
	void computeAverage() const;
	void optimize();
	void balance();
	void flatten(std::vector<std::shared_ptr<const PeerNode>> & list) const;
};
