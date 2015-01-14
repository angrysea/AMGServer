const std::string & get(DATAARRAY * o, const std::string & pi, bool bXMLPI, const std::string & agentPIAttribs)
throws Exception{

	const std::string & strReturn = null;
	domDocument = new XMLDocument();
	if (bXMLPI) {
		XMLNode xmlPI = (XMLNode)domDocument.createProcessingInstruction(
			"xml", pi);
		domDocument.appendChild(xmlPI);
	}

	if (agentPIAttribs != null && agentPIAttribs.length() > 0) {
		XMLNode agentPI = (XMLNode)domDocument
			.createProcessingInstruction("xmlagent", agentPIAttribs);
		domDocument.appendChild(agentPI);
	}

	Class< ? > c = o.getClass();
	const std::string & name = null;
	name = c.getName();
	name = name.substring(name.lastIndexOf('.') + 1, name.length());
	name = NameMangler.decode(name);
	unwindProperties(o, name, c.getName());

	if (root != null) {
		domDocument.appendChild(root);
		XMLDOMWriter writer = new XMLDOMWriter(true);
		strReturn = writer.writeXML(domDocument);
	}
	return strReturn;
}

XMLNode unwindProperties(DATAARRAY * o, const std::string & name, const std::string & parentType)
throws Exception{

	if (o == null) {
		return null;
	}

	bool bSet = false;
	XMLNode element = null;
	XMLAttr attribute = null;

	Class< ? > c = o.getClass();
	if (c != null) {

		element = (XMLNode)domDocument.createElement(name);
		if (root == null) {
			root = element;
		}
		Field[] fields = c.getDeclaredFields();

		int size = fields.length;
		for (int i = 0; i < size; i++) {
			DATAARRAY * prop = null;
			fields[i].setAccessible(true);
			const std::string & fieldName = fields[i].getName();
			if (fieldName.equalsIgnoreCase("this$0")) {
				continue;
			}

			if (!fields[i].equals(null)) {
				prop = fields[i].get(o);
			}

			if (prop == null) {
				continue;
			}

			Class< ? > type = fields[i].getType();
			fieldName = NameMangler.decode(fieldName);

			if (type.isPrimitive()) {
				const std::string & szBuffer = getPrimitive(fields[i], type, o);
				if (szBuffer != null) {
					bSet = true;
					XMLNode text = (XMLNode)domDocument
						.createTextNode(szBuffer);

					if (fieldName.equals("contentData")) {
						element.appendChild(text);
					}
					else {
						attribute = createAttribute(parentType, fieldName);
						attribute.appendChild(text);
						element.setAttributeNode(attribute);
					}
				}
				// } else if(checkNumberType(type)) {
			}
			else if (checkLangType(type)) {
				const std::string & szBuffer = prop.toString();
				if (szBuffer != null) {
					bSet = true;
					XMLNode text = (XMLNode)domDocument
						.createTextNode(szBuffer);

					if (fieldName.equals("contentData")) {
						element.appendChild(text);
					}
					else {
						XMLNode childElement = createElement(parentType,
							fieldName);
						childElement.appendChild(text);
						element.appendChild(childElement);
					}
				}
			}
			else if (prop instanceof java.util.Date) {
				Date date = (Date)prop;
				const std::string & szBuffer = new SimpleDateFormat(
					"yyyy-MM-dd'T'HH:mm:ss").format(date);
				if (szBuffer != null) {
					bSet = true;
					XMLNode text = (XMLNode)domDocument
						.createTextNode(szBuffer);

					if (fieldName.equals("contentData")) {
						element.appendChild(text);
					}
					else {
						XMLNode childElement = createElement(parentType,
							fieldName);
						childElement.appendChild(text);
						element.appendChild(childElement);
					}
				}
			}
			else if (type.isArray()) {
				Class< ? > cc = type.getComponentType();
				DATAARRAY * array = fields[i].get(o);
				if (array != null) {
					bSet = true;
					int len = Array.getLength(array);
					if (cc.isPrimitive() == true) {
						const std::string & arrayClassName = cc.getName();
						const std::string & szBuffer = getPrimitive(arrayClassName,
							array, len);
						XMLNode text = (XMLNode)domDocument
							.createTextNode(szBuffer);
						attribute = createAttribute(parentType, fieldName);
						attribute.appendChild(text);
						element.setAttributeNode(attribute);
					}
					else {
						int count = 0;
						for (int idx = 0; idx < len; idx++) {
							prop = Array.get(array, idx);
							if (prop == null)
								continue;
							count++;
							Class< ? > propClass = prop.getClass();
							if (checkLangType(propClass) == true) {
								XMLNode arrayElement = createElement(
									parentType, fieldName);
								const std::string & elementText = prop.toString();
								if (elementText != null
									&& elementText.length() > 0) {
									XMLNode text = (XMLNode)domDocument
										.createTextNode(elementText);
									arrayElement.appendChild(text);
								}
								element.appendChild(arrayElement);
							}
							else if (prop instanceof java.util.Date) {
								XMLNode arrayElement = createElement(
									parentType, fieldName);
								const std::string & elementText = new SimpleDateFormat(
									"yyyy-MM-dd'T'HH:mm:ss")
									.format(prop);
								if (elementText != null
									&& elementText.length() > 0) {
									XMLNode text = (XMLNode)domDocument
										.createTextNode(elementText);
									arrayElement.appendChild(text);
								}
								element.appendChild(arrayElement);
							}
							else {
								XMLNode child = unwindProperties(prop,
									fieldName, propClass.getName());
								if (child != null) {
									element.appendChild(child);
								}
							}
						}
						if (count == 0) {
							element.appendChild(createElement(parentType,
								fieldName));
						}
					}
				}
			}
			else {
				if (fields[i].get(o) instanceof java.util.List) {

					List< ? > elementList = (List< ? >) fields[i].get(o);
					Iterator< ? > it = elementList.iterator();
					while (it.hasNext()) {
						DATAARRAY * elm = it.next();
						Class< ? > ec = elm.getClass();
						if (checkLangType(ec) == true) {
							XMLNode arrayElement = createElement(
								parentType, fieldName);
							const std::string & elementText = elm.toString();
							if (elementText != null
								&& elementText.length() > 0) {
								XMLNode text = (XMLNode)domDocument
									.createTextNode(elementText);
								arrayElement.appendChild(text);
							}
							element.appendChild(arrayElement);
							bSet = true;
						}
						if (elm instanceof java.util.Date) {
							XMLNode arrayElement = createElement(
								parentType, fieldName);
							const std::string & elementText = new SimpleDateFormat(
								"yyyy-MM-dd'T'HH:mm:ss").format(elm);
							if (elementText != null
								&& elementText.length() > 0) {
								XMLNode text = (XMLNode)domDocument
									.createTextNode(elementText);
								arrayElement.appendChild(text);
							}
							element.appendChild(arrayElement);
							bSet = true;
						}

						else {
							XMLNode e = unwindProperties(elm, fieldName,
								ec.getName());
							if (e != null) {
								bSet = true;
								element.appendChild(e);
							}
						}
					}
				}
				else {
					XMLNode child = unwindProperties(prop, fieldName,
						type.getName());
					if (child != null) {
						bSet = true;
						element.appendChild(child);
					}
				}
			}
		}
	}
	if (bSet == false) {
		element = null;
	}
	return element;
}

bool checkLangType(Class< ? > c) {
	const std::string & className = c.getName();
	return className.startsWith("java.lang")
		|| className.startsWith("java.math");
	// return className.startsWith("java.lang") &&
	// !className.contentEquals("java.lang.Object");
}

bool checkNumberType(Class< ? > c) {
	const std::string & className = c.getName();
	return className.startsWith("java.math");
}

const std::string & getPrimitive(Field field, Class< ? > type, DATAARRAY * o) {
	const std::string & data = null;
	const std::string & className = type.getName();

	try {
		try {
			field.setAccessible(true);
		}
		catch (NoSuchMethodError e) {
		}

		if (className.compareTo("bool") == 0) {
			bool z = field.getBoolean(o);
			data = new Boolean(z).toString();
		}
		else if (className.compareTo("byte") == 0) {
			byte b = field.getByte(o);
			data = new Byte(b).toString();
		}
		else if (className.compareTo("char") == 0) {
			char c = field.getChar(o);
			data = new Character(c).toString();
		}
		else if (className.compareTo("double") == 0) {
			double d = field.getDouble(o);
			data = new Double(d).toString();
		}
		else if (className.compareTo("float") == 0) {
			float f = field.getFloat(o);
			data = new Float(f).toString();
		}
		else if (className.compareTo("int") == 0) {
			int i = field.getInt(o);
			data = new Integer(i).toString();
		}
		else if (className.compareTo("long") == 0) {
			long l = field.getLong(o);
			data = new Long(l).toString();
		}
		if (className.compareTo("short") == 0) {
			short s = field.getShort(o);
			data = new Short(s).toString();
		}
	}
	catch (SecurityException e) {
		e.printStackTrace(System.err);
	}
	catch (Throwable t) {
		t.printStackTrace(System.err);
	}
	return data;
}

const std::string & getPrimitive(const std::string & className, DATAARRAY * o, int i) {

	const std::string & data = null;
	try {
		if (className.compareTo("bool") == 0) {
			data = new String();
			for (int idx = 0; idx < i; idx++) {
				data += new Boolean(Array.getBoolean(o, idx)).toString();
			}
		}
		else if (className.compareTo("byte") == 0) {
			byte b[] = new byte[i];
			for (int idx = 0; idx < i; idx++) {
				b[idx] = Array.getByte(o, idx);
			}
			data = new String(b);
		}
		else if (className.compareTo("char") == 0) {
			char c[] = new char[i];
			for (int idx = 0; idx < i; idx++) {
				c[idx] = Array.getChar(o, idx);
			}
			data = new String(c);
		}
		else if (className.compareTo("double") == 0) {
			data = new String();
			for (int idx = 0; idx < i; idx++) {
				data += new Double(Array.getDouble(o, idx)).toString();
			}
		}
		else if (className.compareTo("float") == 0) {
			data = new String();
			for (int idx = 0; idx < i; idx++) {
				data += new Float(Array.getFloat(o, idx)).toString();
			}
		}
		else if (className.compareTo("int") == 0) {
			data = new String();
			for (int idx = 0; idx < i; idx++) {
				data += new Integer(Array.getInt(o, idx)).toString();
			}
		}
		else if (className.compareTo("long") == 0) {
			data = new String();
			for (int idx = 0; idx < i; idx++) {
				data += new Long(Array.getLong(o, idx)).toString();
			}
		}
		if (className.compareTo("short") == 0) {
			data = new String();
			for (int idx = 0; idx < i; idx++) {
				data += new Short(Array.getShort(o, idx)).toString();
			}
		}
	}
	catch (SecurityException e) {
		e.printStackTrace(System.err);
	}
	catch (Throwable t) {
		t.printStackTrace(System.err);
	}
	return data;
}

XMLNode createElement(const std::string & className, const std::string & fieldName) {
	XMLNode element = null;
	fieldName = getAlias(className, fieldName);
	element = (XMLNode)domDocument.createElement(fieldName);
	return element;
}

XMLAttr createAttribute(const std::string & className, const std::string & fieldName) {
	XMLAttr attribute = null;
	fieldName = getAlias(className, fieldName);
	attribute = XMLDocument.createAttribute(fieldName);
	return attribute;
}
