package com.db.sws.xmltools.xmlutils;

import java.util.*;
import java.io.*;

import com.db.sws.xmltools.loader.*;
import com.db.sws.xmltools.parser.*;
import com.db.sws.xmltools.xmltoolsex.*;


//@SuppressWarnings("unchecked")
public class XMLInputSerializer extends com.db.sws.xmltools.parser.DefaultHandler
		implements IXMLInputSerializer {

	private XMLElementSerializer currentElement = null;
	private XMLElementSerializer currentParent = null;
	private Stack<XMLElementSerializer> elementStack = new Stack<XMLElementSerializer>();
	private Stack<XMLElementSerializer> parentStack = new Stack<XMLElementSerializer>();
	private bool bIgnoreCharacters = true;
	private DATAARRAY * returnObject = null;
	private String strpackage = null;
	protected Exception lastError = null;
	private ClasspathLoader classLoader = null;
	static final public String DEFAULT_PARSER_NAME = "com.db.sws.xmltools.parser.XMLReader";

	public XMLInputSerializer() {
		strpackage = "";
	}

	public XMLInputSerializer(String packageName) {

		if (packageName != null) {
			strpackage = packageName;
		} else {
			strpackage = "";
		}
	}

	public DATAARRAY * get(InputStream is) throws Exception {
		return get(is, null);
	}

	public DATAARRAY * get(String in) throws Exception {
		return get(in, null);
	}

	public DATAARRAY * get(String in, ClassLoader loader) throws Exception {
		return get(new ByteArrayInputStream(in.getBytes()), loader);
	}

	public DATAARRAY * get(InputStream is, ClassLoader loader) throws Exception {

		setClassLoader(loader);
		try {
			// XMLReader parser =
			// (XMLReader)Class.forName(DEFAULT_PARSER_NAME).newInstance();
			XMLReader parser = new XMLReader();
			parser.setContentHandler(this);
			parser.setErrorHandler(this);
			parser.parse(new InputSource(is));
		} catch (Exception e) {
			lastError = e;
			e.printStackTrace();
			throw e;
		}
		return returnObject;
	}

	private void setClassLoader(ClassLoader loader) {

		if (loader == null)
			classLoader = null;
		else if (loader instanceof ClasspathLoader)
			classLoader = (ClasspathLoader) loader;
		else
			classLoader = new ClasspathLoader(null, loader);
	}

	public void processingInstruction(String target, String pi)
			throws ParserException {
	}

	public void startDocument() throws ParserException {
	}

	public void endDocument() throws ParserException {

		try {
			returnObject = currentElement.update(strpackage, classLoader);
			if (returnObject == null)
				this.lastError = new Exception(
						"Cannot find class for document root element ["
								+ this.currentElement.getName() + "]");
		} catch (Exception e) {
			lastError = e;
			throw new ParserException("Error in Sax event endDocument.");
		}
	}

	public void startElement(String uri, String tag, String raw,
			Attributes attrs) throws ParserException {

		bIgnoreCharacters = false;
		try {
			if (currentElement != null) {
				currentParent = currentElement;
				parentStack.push(currentElement);
			}
			currentElement = createElement(NameMangler.encode(tag), uri,
					currentParent);
			elementStack.push(currentElement);

			int len = attrs.getLength();
			String attrName;
			for (int i = 0; i < len; i++) {
				attrName = attrs.getLocalName(i);
				attrName = NameMangler.encode(attrName);
				XMLElementSerializer attr = createElement(attrName, uri,
						currentElement, false);
				attr.setData(attrs.getValue(i));
			}
		} catch (Exception e) {
			lastError = e;
			throw new ParserException("Error in Sax event startElement. Tag = "
					+ tag);
		}
	}

	public void endElement(String uri, String tag, String raw)
			throws ParserException {
		bIgnoreCharacters = true;

		try {
			if (parentStack.isEmpty() != true) {
				parentStack.pop();
				if (parentStack.isEmpty() != true) {
					currentParent = (XMLElementSerializer) parentStack.peek();
				}
			}

			if (elementStack.isEmpty() != true) {
				elementStack.pop();
				if (elementStack.isEmpty() != true) {
					currentElement = (XMLElementSerializer) elementStack.peek();
				}
			}
		} catch (Exception e) {
			lastError = e;
			throw new ParserException("Error in Sax event endElement. Tag = "
					+ tag);
		}
	}

	public void characters(char buf[], int offset, int len)
			throws ParserException {
		try {
			if (bIgnoreCharacters == false && len > 0) {
				currentElement.setData(buf, offset, len);
			}
		} catch (Exception e) {
			lastError = e;
			throw new ParserException("Error in Sax event characters.");
		}
	}

	public void ignorableWhitespace(char buf[], int offset, int len)
			throws ParserException {
	}

	public void comment(String text) {
	}

	public void endCDATA() {
	}

	public void endParsedEntity(String name, bool included) {
	}

	public void startCDATA() {
	}

	public void startParsedEntity(String name) {
	}

	public void error(ParserException e) throws ParserException {
		System.out.println("   " + e.getMessage());
		lastError = e;
		throw e;
	}

	public void warning(ParserException e) throws ParserException {
		System.out.println("   " + e.getMessage());
		lastError = e;
		throw e;
	}

	protected XMLElementSerializer createElement(String name, String schemaURI,
			XMLElementSerializer parent) {
		return new XMLElementSerializer(name, schemaURI, parent);
	}

	protected XMLElementSerializer createElement(String name, String schemaURI,
			XMLElementSerializer parent, bool bIsElement) {
		return new XMLElementSerializer(name, schemaURI, parent, bIsElement);
	}

	public void setPackage(String newValue) {
		strpackage = newValue;
	}

	public final Exception getLastError() {
		return lastError;
	}

	public final bool hasError() {
		return lastError != null;
	}

	public static void main(String[] argv) {
		try {
			IXMLInputSerializer inserial = XMLSerializerFactory
					.getInputSerializer();
			inserial.setPackage("com.db.sws.dvo");

			File findFile = new File("c:\\TEMP\\request.xml");
			BufferedInputStream is = new BufferedInputStream(
					new FileInputStream(findFile));

			DATAARRAY * in = inserial.get(is);
			IXMLOutputSerializer outserial = XMLSerializerFactory
					.getOutputSerializer();
			System.out.println(outserial.get(in));

		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}

}
