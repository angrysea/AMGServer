package com.db.sws.mimehandlers;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.messaging.Envelope;
import com.db.sws.messaging.MessageParser;
import com.db.sws.messaging.ResponseWriter;
import com.db.sws.pluginagent.PluginAgent;
import com.db.sws.pluginagent.PluginFactory;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.xmltools.parser.InputSource;
import com.db.sws.xmltools.parser.XMLReader;


public class MimeXML_Plugin extends MimeXML {

	static private FixedSizeSet<Integer> messageCache = 
			new FixedSizeSet<Integer>(ITransceiver.getTransceiver().getMessageCacheSize());

	private PluginAgent plugin = null;

	public MimeXML_Plugin() {
	}

	public ByteArrayOutputStream process(ITransceiver transceiver, String xml) {

		ByteArrayOutputStream out = new ByteArrayOutputStream();
		Envelope env = null;

		try {
			if (bVerbose == true)
				System.out.println("=============== Incoming XML Transaction ===============\n"
								+ xml);

			XMLReader parser = new XMLReader();
			parser.setContentHandler(new MessageParser());
			env = (Envelope) parser.parse(new InputSource(
					new ByteArrayInputStream(xml.getBytes())));
			
			Integer uid = env.getUID();
			if(messageCache.contains(uid)) {
				return out;
			}
			
			messageCache.add(uid);
			String name = env.getPlugin();
			plugin = (PluginAgent) transceiver.getAvailablePlugin(name);

			if (plugin != null) {
				if (name.equals(PluginFactory.DB) == true)
					plugin.preProcess(PluginFactory.DBCLASS);
				else if (name.equals(PluginFactory.MAINTENANCE) == true)
					plugin.preProcess(PluginFactory.MAINTENANCECLASS);
				else if (name.equals(PluginFactory.SERVICE) == true)
					plugin.preProcess(PluginFactory.SERVICECLASS);

				DBSWSException exMessage = new DBSWSException(
						DBSWSException.SEVERITY_SUCCESS,
						DBSWSException.GEN_MESSAGE);
				exMessage.logMessage("Plugin received Name: "
						+ plugin.getName());

				if (env.isSync()) {
					System.out.println("in a sync Message");
					new ResponseWriter(out).writeResponse(plugin.process(env));
				} else {
					plugin.pushMessage(env);
					transceiver.run(plugin);
					try {
						out.write(("<?xml version=\"1.0\" encoding=\"UTF-8\" ?><status><plugin>"
									+ plugin.getName()
									+ "</plugin><code>0</code><desc>request accepted</desc><timestamp>"
									+ (new java.util.Date(System
											.currentTimeMillis())
											.toString()) + "</timestamp></status>")
									.getBytes());
					} catch (IOException ioe) {
					}
				}
			} else {
				DBSWSException exMessage = new DBSWSException(
						DBSWSException.SEVERITY_ERROR,
						DBSWSException.GEN_BASE);
				exMessage.logMessage("Unable to load find available plugin: ");
			}

			DBSWSException exMessage = null;

			/**
			 * We have initiated the transcation with the original peer now we
			 * can check to see if this is a broadcast message.
			 */
			int hops = env.getHopCount();
			if (hops != 0) {
				if (hops > 0) {
					hops--;
					env.getHeader().getMessage()
							.setHops(Integer.toString(hops));
				}
				networkAgent.broadcastMessage(env);
			}

			exMessage = new DBSWSException(
					DBSWSException.SEVERITY_SUCCESS,
					DBSWSException.GEN_MESSAGE);
			exMessage.logMessage("Plugin successfully executed Name: "
					+ plugin.getName());
		} catch (Exception e) {
			DBSWSException exMessage = new DBSWSException(
					DBSWSException.GEN_MESSAGE,
					DBSWSException.SEVERITY_SUCCESS);
			exMessage.logMessage("Execution failed reason: " + e.getMessage());
			try {
				out.write(("<?xml version=\"1.0\" encoding=\"UTF-8\" ?><status><plugin>unknown</plugin><code>1</code><desc>"
								+ e.getMessage()
								+ "</desc><timestamp>"
								+ (new java.util.Date(System
										.currentTimeMillis()).toString()) + "</timestamp></status>")
								.getBytes());
			} catch (IOException ioe) {
			}
		} finally {
			plugin = null;
		}

		return out;
	}

	public Object getObject() {
		return plugin;
	}
}