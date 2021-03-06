package com.db.sws.pluginagent;

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.dbswsex.PluginException;
import com.db.sws.messaging.Address;
import com.db.sws.messaging.Envelope;
import com.db.sws.messaging.Message;
import com.db.sws.messaging.Messenger;
import com.db.sws.registry.PluginEntry;

public final class MaintenancePlugin extends SystemPlugin {

	static private Map<String, MaintenanceWorker> workers = Collections
			.synchronizedMap(new HashMap<String, MaintenanceWorker>(10));

	public void startPlugin(PluginEntry entry) throws Exception {
	}

	public boolean preProcessMessage(Envelope env) {
		return true;
	}

	public Object process(Envelope env) throws Exception {

		try {
			if (env.isMethod("ping")) {
				ping(env.getHeader().getMessage());
			} else if (env.isMethod("pong")) {
				pong(env.getHeader().getMessage());
			}
		} catch (Exception e) {
			PluginException agentex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS);
			agentex.logMessage("Method not supported by Deutsche Bank Maintenance Agent. "
					+ e.getMessage());
			throw e;
		}
		return null;
	}

	public void ping(Message msg) {

		try {
			Message message = Message.createReply(msg);
			message.setMethod("pong");
			Messenger.postMessage(message);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void pong(Message msg) {

		try {
			MaintenanceWorker worker = workers.get(msg.getReplyTo().getURL());
			worker.setResponded(true);
			worker.setEndTime(System.currentTimeMillis());

			// System.out.println("Response time for peer " +
			// worker.getAddress().getURL() + " is " +
			// Long.toString(worker.getPingTime()));
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public String process(String xml) throws Exception {

		PluginException agentex = new PluginException(
				DBSWSException.SEVERITY_FATAL,
				PluginException.ANT_OBJDOTRANS);
		agentex.logMessage(agentex);
		throw agentex;
	}

	static public void doPing(Address address) {

		MaintenanceWorker worker = new MaintenanceWorker(address);
		workers.put(address.getURL(), worker);
	}

	static public Collection<MaintenanceWorker> workers() {
		return workers.values();
	}

	static public void clear() {
		workers.clear();
	}

}
