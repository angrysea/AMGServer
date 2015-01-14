package com.db.sws.pluginagent;

import com.db.sws.messaging.Address;
import com.db.sws.messaging.Message;
import com.db.sws.messaging.Messenger;
import com.db.sws.transceiver.ITransceiver;


public final class MaintenanceWorker extends Worker{

	public MaintenanceWorker(Address address) {
		super(address);
	}

	public void doPing() {
		Message message = null;

		try {
			message = new Message(address.getURL() + "/" + 
					PluginFactory.MAINTENANCE + "/ping",
					ITransceiver.getTransceiver());
			message.getReplyTo().setPlugin(PluginFactory.MAINTENANCE);
			bresponded = false;
			starttime = System.currentTimeMillis();
			Messenger.sendMessage(message);
		} catch (Exception e) {
			starttime = 0;
			endtime = 0;
		}
	}
}
