package com.db.sws.plugins;

import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;

import com.db.sws.messaging.Address;
import com.db.sws.messaging.Message;
import com.db.sws.peer.PeerEntry;
import com.db.sws.peer.PeerNode;
import com.db.sws.pluginagent.Plugin;
import com.db.sws.transceiver.NetworkAgent;

public class Console extends Plugin {
	boolean packFrame = false;
	ConsoleFrame frame = null;
	java.util.Map<String, Long> pingMap = Collections
			.synchronizedMap(new HashMap<String, Long>());
	NetworkAgent peers = null;

	public Console() {
	}

	public void peerUpdate() {
		if (frame != null) {
			frame.clearPeers();
			if (peers != null) {
				for (PeerNode node : peers.getPeers()) {
					try {
						PeerEntry entry = node.getEntry();
						frame.insertPeer(entry.getAddress().getHost() + ":"
								+ entry.getAddress().getPort());
					} catch (NullPointerException e) {						
					}
				}
			}
		}
	}

	public void init() {
		frame = new ConsoleFrame();

		if (packFrame)
			frame.pack();
		else
			frame.validate();
		frame.setVisible(true);
		frame.setPlugin(this);

		// Grab the peers out of the peer list
		PeerEntry entry = null;
		peers = (NetworkAgent) transceiver.getService("networkagent");
		Iterator<PeerNode> it = peers.getValues(true);
		while (it.hasNext()) {
			entry = it.next().getEntry();
			try {
				frame.insertPeer(entry.getAddress().getHost() + ":"
						+ entry.getAddress().getPort());
			} catch (NullPointerException e) {
			}
		}
		frame.init(transceiver.getHost() + ":"
				+ Integer.toString(transceiver.getPort()));
	}

	public void cleanup() {
	}

	public void error(String uri, String msg) {
		Address address = new Address(uri);
		String text = "Error pinging " + address.getURL() + " reason " + msg;
		frame.appendStatus(text);
	}

	public void ping() {
		Message message = null;
		try {
			String text = "Ping from: " + msg.getAddress().getURL();
			frame.appendStatus(text);
			message = Message.createReply(msg);
			message.setMethod("pong");
			postMessage(message);
		} catch (Exception e) {
			frame.appendStatus("Peer " + message.getAddress().getURI()
					+ " not reachable during ping reply.");
		}
	}

	public void pong() {
		try {
			String text = null;
			long pingtime = System.currentTimeMillis()
					- pingMap.get(msg.getReplyTo().getURL()).longValue();
			text = "Pong from: " + msg.getURL() + " milliseconds: "
					+ Long.toString(pingtime);
			frame.appendStatus(text);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void doPing(String to) {
		Message message = null;
		pingMap.clear();

		try {
			if (to.startsWith("http://") == false)
				to = "http://" + to;
			message = new Message(to + "/Console/ping", transceiver);
			message.getReplyTo().setPlugin("Console");
			pingMap.put(message.getURL(), new Long(System.currentTimeMillis()));
			/*
			 * Object args[] = new Object[1]; String data[] = new String[3];
			 * data[0] = "String 1"; data[1] = "String 2"; data[2] = "String 3";
			 * args[0] = data;
			 */
			postMessage(message);
		} catch (Exception e) {
			frame.appendStatus("Peer " + message.getAddress().getURI()
					+ " not reachable during ping.");
		}
	}
}