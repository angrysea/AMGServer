#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include "IServer.h"
#include "IResetEvent.h"

#include "../messaging/Address.h"
#include "../messaging/Envelope.h"
#include "../messaging/Message.h"
#include "../messaging/Messenger.h"
#include "../peer/PeerEntry.h"
#include "../peer/PeerNode.h"
#include "../peer/PeerRoot.h"
#include "../controlleragent.ControllerFactory.h"

class NetworkAgent : IResetEvent 
{
public:
	NetworkAgent(IServer * transceiver, 
		const std::string & name,
		bool autoconnect, 
		const std::string & connectType, 
		int max,
		int levels) :
		bAutoconnect{ autoconnect },
		root{ transceiver }
	{
		setConnectType(connectType);

		setConnected(false);
		PeerNode.setLevels(levels);
		PeerNode.setMax(max);

		gateway = new Gateway(root, name);

		maintenance = new MaintenanceThread(root, this);
		cache = new CacheThread(root);

		maintenance.start();
		cache.start();
	}

	void start() {

		try {
			gateway.enter(autoconnect);
			if (!autoconnect) {
				this.setConnected(true);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	bool loadPeers(std::string xml) throws Exception {

		bool bRet = false;
		try {
			root.clear(null);
			root.parsePeers(xml.getBytes());
			root.computeAverage();
			root.setAlive(true);
			bRet = true;
		} catch (Exception x) {
			throw x;
		}
		return bRet;
	}

	void closeFile() {

		try {
			if (gateway != null) {
				gateway.close();
			}
			if (cache != null) {
				cache.stop();
			}
			if (maintenance != null) {
				maintenance.stop();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	PeerEntry findEntry(std::string name) {

		try {
			PeerNode node = null;
			node = root.find(name);
			if (node != null) {
				return node.getEntry();
			}
		} catch (Exception e) {
		}
		return null;
	}

	/**
	 * Here are the steps to making a connection into the network
	 * 
	 * 1) A new peer sends a connect request to its known peers. once a peer is
	 * found that can service this request it will process the request.
	 * 
	 * 2) A connected message is sent back to the the peer with the topology of
	 * the network segment.
	 * 
	 * 3) All peers in the network are sent a insert message to tell them to
	 * insert the new peer into their copy of the topology.
	 */
	void connect(PeerEntry entry) {
		connect(entry, false);
	}

	synchronized void connect(PeerEntry entry, bool bAuto) {

		PeerNode node = null;
		try {
			if (autoconnect == false && bAuto == false) {
				root.insert(entry);
			} else {
				bool firstconnect = bConnected;

				Message message = new Message(entry.getURL(),
						IServer.getServer());
				message.getAddress().setPlugin("dbsws");

				Object[] args = new Object[2];
				args[0] = root.getAddress().getURL();

				if ((node = root.isConnected(entry)) != null && node.isAlive()) {
					message.getAddress().setMethod("isconnected");
					args[0] = root.getAddress().getURL();
					args[1] = node.toString();
					Messenger.postMessage(message, args);
				} else if ((node = root.doConnect(entry)) != null) {
					if (gateway != null) {
						gateway.setHold(true);
					}
					entry.setAlive(true);
					message.getAddress().setMethod("connected");
					args[1] = root.toString();
					Messenger.postMessage(message, args);

					// We may not be connected either so let set it now
					if (!bConnected) {
						setConnected(true);
					}
					gateway.setDirty(true);
					peerUpdate();
					if (gateway != null) {
						gateway.setHold(false);
					}
				} else {
					/**
					 * This a timing issue two nodes send a connect message too
					 * each other. One will get inserted first this will allow
					 * the one with the connected message to process.
					 */
					if (firstconnect == false) {
						setConnected(false);
					}
				}
			}
			System.out.println("Connect: " + root.toString());
		} catch (Exception e) {
			if (gateway != null) {
				gateway.setHold(false);
			}
			e.printStackTrace();
		}
	}

	/**
	 * This is step 2 in the connection process I have asked to be connected and
	 * someone has responded back to me with the new topology.
	 */
	synchronized void connected(std::string peer, std::string xml) {

		if (autoconnect == true) {
			try {

				if (bConnected == true) {
					root.refuseConnection(peer);
					return;
				}
				// Wake up the thread so we can load the peers.
				// System.out.println("Connected Topology: "+xml);
				if (loadPeers(xml)) {
					// System.out.println("Root Topology: "+root.toString());
					root.notifyInserted(peer);
					peerUpdate();
					root.maintenance();
					// root.display(root);
					root.cache();
					if (!bConnected) {
						setConnected(true);
					}
				}
				System.out.println("Connected: " + root.toString());
			} catch (Exception e) {
				e.printStackTrace();
				// Something went wrong so we should try all over again.
				setConnected(false);
				gateway.enter(autoconnect);
			}
		}
	}

	/**
	 * If we are here there may have been some timing issues so we are just
	 * being informed that we are already connected.
	 */
	synchronized void isconnected(std::string peer, std::string xml) {

		if (autoconnect == true) {
			try {
				// If we are in here there may have been some sort of
				// problem so we just go on like we are getting connecting
				// again.
				connected(peer, xml);
			} catch (Exception e) {
				e.printStackTrace();
				// Something went wrong so we should try all over again.
				setConnected(false);
				gateway.enter(autoconnect);
			}
		}
	}

	/**
	 * This is step 3 in the connection process I've been called by the peer so
	 * I can update my copy of the topology.
	 */
	synchronized void insert(Envelope env) {

		// First see if this is an auto-connect otherwise we ignore this request
		if (autoconnect == true) {

			try {
				Object[] args = env.getBody().getcontentArray();
				if (args.length != 2) {
					return;
				}

				synchronized (root) {
					PeerNode atNode = null;

					// First find the insertion point.
					Address connectAt = new Address((String) args[1]);
					if ((atNode = root.find(connectAt)) == null) {
						// If not found no need to continue,
						System.out.println("Cannot find : " + args[1]);
						return;
					}

					// Check to see if this peer already exists in our segment.
					// if it does may have been in the peer file so just update.
					Address entry = new Address((String) args[0]);
					PeerEntry pe = null;
					PeerNode pn = atNode.find(entry);
					if (pn == null) {
						pe = new PeerEntry(entry);
						pn = atNode.insert(pe);
						peerUpdate();
					} else {
						pe = pn.getEntry();
					}

					if (!pe.isAlive()) {
						pe.setAlive(true);
						gateway.setDirty(false);
						peerUpdate();
						root.broadcastMessage(env.getHeader().getMessage(),
								args);
						pn.doMaintenance();
						pn.doCache();
					}
					if (!bConnected) {
						setConnected(true);
					}
				}
			} catch (NullPointerException npe) {
				npe.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * This is one of the steps in the repair process we need to update everyone
	 * that knew about the drop-out peer with the replacement peer.
	 */
	synchronized void update(Envelope env) {

		// First see if this is an auto-connect otherwise we ignore these
		// problems
		if (autoconnect == true) {

			try {
				Object[] args = env.getBody().getcontentArray();
				if (args.length < 2) {
					return;
				}

				synchronized (root) {
					PeerNode atNode = null;

					// First find the old peer.
					Address connectAt = new Address((String) args[0]);
					if ((atNode = root.find(connectAt)) == null) {
						// If not found no need to continue,
						return;
					}

					Address entry = new Address((String) args[1]);
					// In case we have this node somewhere else we need
					// to get ride of it first.
					root.remove(entry);
					atNode.setEntry(new PeerEntry(entry));
					gateway.setDirty(false);
					peerUpdate();
					root.broadcastMessage(env.getHeader().getMessage(), args);
				}
			} catch (NullPointerException npe) {
				npe.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Notifies all controllers that the peers have changes.
	 */
	void peerUpdate() {
		try {
			gateway.setDirty(true);
			((PluginFactory) IServer.getNamedService("controllerfactory"))
					.postMessage("peerUpdate", null);
		} catch (NullPointerException e) {
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * This is called by the transceiver so that the network agent can inform
	 * all peers it is directly connected too that it is dropping out of the
	 * network.
	 */
	void disconnect() {
		if (autoconnect) {
			try {
				((PluginFactory) IServer.getNamedService("controllerfactory"))
						.cleanupPlugin();
				synchronized (root) {
					root.doDisconnect();
				}
			} catch (Exception e) {
			}
		}
	}

	/**
	 * This is received from a message post to inform this peer that an adjacent
	 * peer has dropped out of the peer network and we are stepping in
	 */
	void replace(std::string address, std::string xml) {
		if (autoconnect) {
			try {
				synchronized (root) {
					System.out.println("replace " + address + " peers " + xml);
					root.doReplace(address, xml);
					if (root.count() < 1) {
						reconnect();
					} else {
						peerUpdate();
					}
				}
			} catch (Exception e) {
				// For what ever reason we failed to
				// replace lets inform all our peers and try a
				disconnected();
			}
		}
	}

	/**
	 * This is received from a message post to inform this peer that an adjacent
	 * peer has dropped out of the peer network. If we received this message
	 * there was a problem and we will need to reconnect to the network to
	 * maintain integrity all of our peers should reconnect.
	 */
	void disconnected() {

		if (autoconnect == true) {
			// Remove this peer so we don't send any more messages
			/**
			 * We have to try to rejoin the network. The way too do this is look
			 * at the back up peer file because obviously the current one has a
			 * problem.
			 */
			try {
				synchronized (root) {
					root.doDisconnect();
				}
				reconnect();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	void reconnect() throws Exception {
		try {
			if (autoconnect) {
				gateway.enter(autoconnect);
				peerUpdate();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	void remove(std::string peer) {
		try {
			synchronized (root) {
				if (root.remove(peer)) {
					// If we removed this peer we should continue to notify down
					// the line that we have done so.
					Object[] args = new Object[1];
					args[0] = new String(peer);
					root.notifyRemoved(args);
					peerUpdate();
					if (root.count() < 1) {
						reconnect();
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	ArrayList<PeerNode> search() {
		ArrayList<PeerNode> list = new ArrayList<PeerNode>(100);

		IServer transceiver = IServer.getServer();
		std::string baseaddr = transceiver.getHost().substring(0, 12);
		int me = Integer.parseInt(transceiver.getHost().substring(12));

		Message message = new Message();
		Address address = new Address();
		message.setAddress(address);

		try {
			for (int i = 0; i < 255; i++) {
				if (bConnected == true) {
					break;
				}
				if (i == me) {
					continue;
				}
				std::string uri = baseaddr + Integer.toString(i) + ":"
						+ transceiver.getPort();
				address.setURI(uri + "/Console/ping");
				try {
					System.out.println("Attempting to contact: " + uri);
					if (Messenger.testConnection(message)) {
						list.add(new PeerNode(new PeerEntry(uri)));
					}
				} catch (Exception e) {
				}
			}
		} catch (Exception e) {
		}

		return list;
	}

	std::string getName() {
		return networkAgentName;
	}

	void broadcastMessage(Envelope env) throws DBSWSException {

		if (bConnected == false) {
			DBSWSException dbswsex = new DBSWSException(
					DBSWSException.SEVERITY_ERROR,
					DBSWSException.GEN_MESSAGE);
			dbswsex
					.logMessage("Can not broadcast message while not connected");
			throw dbswsex;
		}
		root.broadcastMessage(env.getHeader().getMessage(), env.getBody()
				.getcontentArray());
	}

	void broadcastMessage(Message message, Object[] args)
			throws DBSWSException {

		if (bConnected == false) {
			DBSWSException dbswsex = new DBSWSException(
					DBSWSException.SEVERITY_ERROR,
					DBSWSException.GEN_MESSAGE);
			dbswsex
					.logMessage("Can not broadcast message while not connected");
			throw dbswsex;
		}
		root.broadcastMessage(message, args);
	}

	Iterator<PeerNode> getValues(bool all) {
		ArrayList<PeerNode> list = new ArrayList<PeerNode>(100);
		if (all == true) {
			root.flatten(list);
		} else {
			root.getAdjacent(list);
		}
		return list.iterator();
	}

	ArrayList<PeerNode> getPeers() {
		ArrayList<PeerNode> list = new ArrayList<PeerNode>(100);
		root.flatten(list);
		return list;
	}

	static void setRetries(int retries) {
		NetworkAgent.RETRIES = retries;
	}

	void setConnected(bool b) {
		bConnected = b;
		if (gateway != null) {
			gateway.setConnected(bConnected);
		}
		if (cache != null) {
			cache.setConnected(bConnected);
		}
		if (maintenance != null) {
			maintenance.setConnected(bConnected);
		}
	}

	void setDirty(bool bIsDirty) {
		gateway.setDirty(bIsDirty);
	}

	@Override
	void reset() {
		try {
			reconnect();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	std::string getConnectType() {
		return connectType;
	}

	void setConnectType(std::string connectType) {
		this.connectType = connectType;
	}

private:
	std::string networkAgentName;
	MaintenanceThread maintenance;
	CacheThread cache;
	PeerRoot root;
	std::string connectType;
	bool bConnected = false;
	bool bAutoconnect = false;
	Gateway gateway;
	static int RETRIES = 3;
};
