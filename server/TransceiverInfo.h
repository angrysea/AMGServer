package com.db.sws.transceiver;

import java.util.Properties;

public class TransceiverInfo {
	static public TransceiverProperties properties;

	static public boolean bVerbose = false;

	static public boolean bAutoReload = false;

	public static final String VERSION = "Transceiver version 0.91 Beta";

	static public Properties getServerProperty() {
		return properties;
	}

	static public String getServerProperty(String name) {
		return properties.getProperty(name);
	}

}
