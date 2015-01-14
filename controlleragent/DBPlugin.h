package com.db.sws.pluginagent;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.dbswsex.PluginException;
import com.db.sws.loader.ClasspathLoader;
import com.db.sws.messaging.Body;
import com.db.sws.messaging.Envelope;
import com.db.sws.messaging.Message;
import com.db.sws.messaging.Messenger;
import com.db.sws.peer.PeerEntry;
import com.db.sws.registry.PluginEntry;
import com.db.sws.registry.PluginFile;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiver.NetworkAgent;

final class DBPlugin extends SystemPlugin {

	private NetworkAgent networkAgent = null;
	
	public void startPlugin(PluginEntry entry) throws Exception {
	}

	public void init(ClasspathLoader loader, PluginAgent agent) {
		super.init(loader, agent);
		networkAgent = (NetworkAgent) ITransceiver.getTransceiver().getService(
				"networkAgent");
	}

	public void error(String uri, String errorMsg) {
		DBSWSException exMessage = new DBSWSException(
				DBSWSException.SEVERITY_ERROR, DBSWSException.GEN_BASE);
		exMessage.logMessage("Error processing message : " + uri
				+ "\n Error message " + errorMsg);
	}

	public boolean preProcessMessage(Envelope env) {
		return true;
	}

	public Object process(Envelope env) throws Exception {
		try {
			if (env.isMethod("connect")) {
				connect(env.getBody());
			} else if (env.isMethod("connected")) {
				connected(env.getBody());
			} else if (env.isMethod("isconnected")) {
				isconnected(env.getBody());
			} else if (env.isMethod("replace")) {
				replace(env.getBody());
			} else if (env.isMethod("getkey")) {
				Message message = Message.createReply(env.getHeader()
						.getMessage());
				Object args[] = new Object[1];
				args[0] = new String(ITransceiver.getKey());
				Messenger.postMessage(message, args);
			} else if (env.isMethod("reconnect")) {
				reconnect();
			} else if (env.isMethod("insert")) {
				networkAgent.insert(env);
			} else if (env.isMethod("update")) {
				networkAgent.update(env);
			} else if (env.isMethod("remove")) {
				remove(env.getBody());
			} else if (env.isMethod("load")) {
				load(env.getBody());
			} else if (env.isMethod("peerUpdate")) {
				peerUpdate();
			}
			else if (env.isMethod("error")) {
				Object args[] = env.getBody().getcontentArray();
				error((String) args[0], (String) args[1]);
			}
		} catch (Exception e) {
			e.printStackTrace();
			PluginException agentex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS);
			agentex.logMessage("Method not supported by Deutsche Bank Command Agent. "
							+ e.getMessage());
			throw e;
		}
		return null;
	}

	private void connect(Body body) throws Exception {

		Object args[] = body.getcontentArray();
		if (args.length != 3)
			throw new Exception("Out of Bounds");
		// Address of the peer to connect
		PeerEntry entry = new PeerEntry((String) args[0]);
		// Common name for the peer.
		entry.setName((String) args[1]);
		entry.setTime(Long.toString(System.currentTimeMillis()));
		networkAgent.connect(entry, ((Boolean) args[2]).booleanValue());
	}

	private void connected(Body body) throws Exception {

		Object args[] = body.getcontentArray();
		if (args.length != 2)
			throw new Exception("Out of Bounds");
		networkAgent.connected((String) args[0], (String) args[1]);
	}

	private void isconnected(Body body) throws Exception {

		Object args[] = body.getcontentArray();
		if (args.length != 2)
			throw new Exception("Out of Bounds");
		networkAgent.isconnected((String) args[0], (String) args[1]);
	}

	private void replace(Body body) throws Exception {

		Object args[] = body.getcontentArray();
		if (args.length != 2)
			throw new Exception("Out of Bounds");
		networkAgent.replace((String) args[0], (String) args[1]);
	}

	private void reconnect() throws Exception {

		networkAgent.reconnect();
	}

	private void remove(Body body) throws Exception {

		Object args[] = body.getcontentArray();
		if (args.length < 1)
			throw new Exception("Out of Bounds");
		networkAgent.remove((String) args[0]);
	}

	private void load(Body body) throws Exception {
		Object args[] = body.getcontentArray();
		if (args.length < 2)
			throw new Exception("Out of Bounds");
		// networkAgent.update();
		PluginAgent plugin = (PluginAgent) ITransceiver.getTransceiver()
				.getAvailablePlugin((String) args[0]);
		plugin.preProcess("com.db.sws.pluginagent.ServicePlugin");

		try {
			PluginFile plugins = (PluginFile) ITransceiver.getTransceiver()
					.getService("pluginfile");
			plugin.startPlugin(plugins.findEntry((String) args[1]));
		} catch (Exception e) {
		}
	}

	private void peerUpdate() throws Exception {

		try {
			ITransceiver.getTransceiver().getService("pluginfactory");
			// plugin.startPlugin(plugins.findEntry((String)args[1]));
		} catch (Exception e) {
		}
	}

	public String process(String xml) throws Exception {
		PluginException agentex = new PluginException(
				DBSWSException.SEVERITY_FATAL,
				PluginException.ANT_OBJDOTRANS);
		agentex.logMessage(agentex);
		throw agentex;
	}

	public boolean useEnvelope() {
		return true;
	}
}
