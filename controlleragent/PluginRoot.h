package com.db.sws.pluginagent;

import com.db.sws.dbswsex.PluginException;
import com.db.sws.messaging.Message;
import com.db.sws.transceiver.ITransceiver;


abstract public class PluginRoot {

	protected ITransceiver transceiver = null;

	protected Message msg = null;

	protected PluginAgent agent = null;

	public PluginRoot() {
		transceiver = ITransceiver.getTransceiver();
	}

	public abstract void init();

	public abstract void cleanup();

	public abstract Object sendMessage(Message message, Object ... args)
	throws PluginException;
	
	public abstract void broadcastMessage(String toUri, Object... args)
			throws PluginException;

	public abstract void broadcastMessage(String toUri,
			ITransceiver replyTransceiver, Object... args)
			throws PluginException;

	public abstract void postMessage(Message message) throws PluginException;

	public abstract void postMessage(Message message, Object... args)
			throws PluginException;

	public abstract void localPostMessage(Message message, Object... args)
			throws PluginException;

	public abstract void broadcastMessage(Message message, Object... args)
			throws PluginException;

	final void setCurrentMessage(Message msg) {
		this.msg = msg;
	}

	final void setAgent(PluginAgent agent) {
		this.agent = agent;
	}

	final void setTransceiver(ITransceiver transceiver) {
		this.transceiver = transceiver;
	}

	public void unloadPlugin() {
		ITransceiver.getTransceiver().killRequest(agent.getName(), true);
	}

	public void shutdownTransceiver() {
		new Thread(new Runnable() {
			public void run() {
				try {
					Thread.sleep(50);
				} catch (InterruptedException e) {
				}
				try {
					transceiver.shutdown();
					System.exit(1);
				} catch (Exception e) {
				}
			}
		}).start();

	}
}
