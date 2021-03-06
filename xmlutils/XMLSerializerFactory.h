package com.db.sws.xmltools.xmlutils;

public class XMLSerializerFactory {
	/**
	 * A private constructor is defined to prevent instantiation of a factory
	 * object.
	 */
	private XMLSerializerFactory() {
	}

	/**
	 * This static method will create a proper input serializer for you and
	 * return the interface to you.
	 */
	public static IXMLInputSerializer getInputSerializer() throws Exception {
		IXMLInputSerializer in = (IXMLInputSerializer) Class.forName(
				"com.db.sws.xmltools.xmlutils.XMLInputSerializer")
				.newInstance();
		return in;
	}

	/**
	 * This static method will create a proper input serializer for you and
	 * return the interface to you.
	 */
	public static IXMLOutputSerializer getOutputSerializer() throws Exception {
		IXMLOutputSerializer out = (IXMLOutputSerializer) Class.forName(
				"com.db.sws.xmltools.xmlutils.XMLOutputSerializer")
				.newInstance();
		return out;
	}

	public static IXMLOutputSerializer getOutputSerializerVerbose()
			throws Exception {
		IXMLOutputSerializer out = (IXMLOutputSerializer) Class.forName(
				"com.db.sws.xmltools.xmlutils.XMLOutputSerializerVerbose")
				.newInstance();
		return out;
	}

	static {
		try {
			Class.forName("java.lang.reflect.AccessibleObject");
		} catch (ClassNotFoundException cnfe) {
		}
	}
}
