#include "PeerRoot.h"
#include "PeerTree.h"

PeerRoot::PeerRoot(const std::string & host, int port) :
PeerNode{ PeerEntry{ host, port, false } }
{
	peerEntry.setName(host);
}

PeerRoot::PeerRoot(const PeerEntry & peerEntry) : PeerNode{ peerEntry }
{
	
}

PeerRoot::PeerRoot() 
{
}

void PeerRoot::initRoot() 
{
	//messenger = new Messenger();
}

std::shared_ptr<const PeerNode> PeerRoot::isConnected(const PeerEntry & entry) const
{
	if (entry.getURL()==getURL()) 
	{
		return nullptr;
	}
	// Check to see if this peer already exists in our segment.
	return tree->find(entry.getURL(), std::shared_ptr<const PeerNode>{this});
}

/**
* When finding a node to insert a lot has to be determined.
*/
std::shared_ptr<const PeerNode> PeerRoot::doConnect(const PeerEntry & entry) 
{
	/**
	* Now we need to find the peer that is willing to accept the request
	*
	* First try to find a node that is within the average and increment
	* up until max;
	*/
	auto node = getConnectionNode();

	// Check to see if we are the insertion node.
	if (*node == *this) 
	{
		return insert(entry);
	}

	/**
	* If no one volunteers to take this peer we will have to go outside
	* our segment. Two help balance lets find a leave with the lowest
	* count.
	*/
	if (node == nullptr) 
	{
		std::vector<std::shared_ptr<const PeerNode>> list;
		tree->getLeaves(list, std::shared_ptr<const PeerNode>{this});
		for (auto child : list)
		{
			unsigned int lowest = SHRT_MAX;
			if (lowest > child->size()) 
			{
				lowest = child->size();
				node = child;
			}
		}
		// The node with the lowest count should fall through
	}

	// Most likely candidate for accepting the request.
	if (node != nullptr) 
	{
		node->postMessage("dbsws", "connect", entry.getURL(), entry.getName(), true);
	}
	return nullptr;
}

std::shared_ptr<const PeerNode> PeerRoot::getConnectionNode() const
{
	unsigned int inc = 0;
	std::shared_ptr<const PeerNode> node = nullptr;
	while (inc + average <= max) 
	{
		/**
		* Before we go crazy maybe I'm the insertion node Its a simple
		* Check so lets do it first;
		*/
		if (inc + average <= count()) 
		{
			return std::shared_ptr<const PeerNode>{this};
		}
		/**
		* This must be full so We'll have to check to see who can do
		* the connection.
		*/
		if ((node = tree->getConnectionNode(inc, std::shared_ptr<const PeerNode>{this})) != nullptr)
		{
			return node;
		}
		inc++;
	}
	return nullptr;
}

void PeerRoot::maintenance() 
{
	if (isAlive()) 
	{
		std::vector<std::shared_ptr<const PeerNode>> list;
		tree->flatten(list, std::shared_ptr <const PeerNode>{ this });
		for (auto node : list) 
		{
			node->doMaintenance();
		}
	}
}

void PeerRoot::cache() {

	if (isAlive())
	{
		std::vector<std::shared_ptr<const PeerNode>> list;
		tree->flatten(list, std::shared_ptr <const PeerNode>{ this });
		for (auto node : list)
		{
			node->doCache();
		}
	}
}

void PeerRoot::refuseConnection(const std::string & peer) const
{
	// Check to see if this peer is in our segment anyway.
	if (tree->find(peer, std::shared_ptr<const PeerNode>{this}) != nullptr)
		return;

	//Message message = new Message(peer);
	//message.getAddress().setPlugin("dbsws");
	//message.getAddress().setMethod("remove");
	//Messenger.postMessage(message, peerEntry.getAddress().getURL());
}

/**
* This is called through the NetworkAgent and is initiated by the
* server form shutdown to disconnect from the network
*/
void PeerRoot::doDisconnect()
{
	// Get the message ready.
	//tree->broadcastMessage("dbsws", "remove", getEntry().getURL());
}

/**
* This is called through the NetworkAgent to inform this peer that an
* adjacent peer has dropped out of the peer network and we are stepping in
*/
void PeerRoot::doReplace(const std::string & address, const std::string & xml)
{
	Address replaceAt{ address };
	remove(replaceAt);
	std::vector<std::shared_ptr<const PeerNode>> list;
	tree->getNodes(list);

	// First we need to find our replacement
	for (std::shared_ptr<const PeerNode> node : list) 
	{
		//Message message = new Message(node.getEntry().getURL());
		//message.getAddress().setPlugin("dbsws");
		//message.getAddress().setMethod("replace");
		//message.setKey(peerEntry.getKey());

		remove(node->getAddress());

		// Post message to our sub
		//node.postMessage(message, peerEntry.getURL(), toString());
	}

	
	// Update our new structure
	parsePeers(xml);

	//Message message = new Message();
	//message.getAddress().setPlugin("dbsws");
	//message.getAddress().setMethod("update");
	//message.setKey(peerEntry.getKey());

	// Prepare the parameters
	//broadcastMessage(message, address, peerEntry.getURL());
}

/**
* This will be called by an adjacent peer when it is dropping out of the
* network or being optimized out.
*/
void PeerRoot::parsePeers(const std::string & bytes)
{
	if (bytes.length() > 0) 
	{
		//XMLReader parser;
		//parser.setContentHandler(PeerParser(this));
		//parser.parse(new InputSource(new ByteArrayInputStream(bytes)));
	}
}

void PeerRoot::setAlive(bool bAlive) 
{
	peerEntry.setAlive(bAlive);
	tree->setAlive(std::shared_ptr<const PeerNode>{this}, bAlive);
}

unsigned int PeerRoot::count() const 
{
	return PeerNode::count(nullptr);
}

int PeerRoot::getRoute(const std::string & tag, std::vector<std::shared_ptr<const PeerNode>>  & list) const
{
	return tree->getRoute(tag, list, std::shared_ptr<const PeerNode>{ this });
}

int PeerRoot::getRoute(const Address & tag, std::vector<std::shared_ptr<const PeerNode>> & list) const
{
	return tree->getRoute(tag, list, std::shared_ptr<const PeerNode>{ this });
}

int PeerRoot::getPath(const std::string & tag, std::string & sPath) const
{
	return tree->getPath(tag, sPath, std::shared_ptr<const PeerNode>{ this });
}

int PeerRoot::getPath(const Address & tag, std::string & sPath) const
{
	return tree->getPath(tag, sPath, std::shared_ptr<const PeerNode>{ this });
}

void PeerRoot::getLeaves(std::vector<std::shared_ptr<const PeerNode>> & list) const 
{
	tree->getLeaves(list, std::shared_ptr<const PeerNode>{ this });
}

void PeerRoot::computeAverage() const
{
	PeerNode::average = count();
	if (PeerNode::average > 0) 
	{
		PeerNode::average /= 2;
	}
	tree->computeAverage(std::shared_ptr<const PeerNode>{ this });
}

void PeerRoot::optimize() 
{
	tree->optimize(std::shared_ptr<const PeerNode>{ this });
}

void PeerRoot::balance() 
{
	tree->balance(std::shared_ptr<const PeerNode>{ this });
}

void PeerRoot::flatten(std::vector<std::shared_ptr<const PeerNode>> & list) const
{
	tree->flatten(list, std::shared_ptr<const PeerNode>{ this });
}
