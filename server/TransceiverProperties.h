package com.db.sws.transceiver;

import java.util.Properties;

public abstract class TransceiverProperties extends Properties {

	private static final long serialVersionUID = 821655720798197367L;

	static public TransceiverProperties getInstance(String name) {
		TransceiverProperties o = null;
		try {
			Class<?> c = Class.forName(name);
			o = (TransceiverProperties)c.newInstance();

		} catch (Exception e) {
			o = null;
		}
		return o;
	}
}
