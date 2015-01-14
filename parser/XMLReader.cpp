#include <fstream>
#include <exception>

#include "XMLReader.h"

DATAARRAY * XMLReader::parse(std::istream * in)
{
	DataArrayFree(obj);
	is = in;
	try 
	{
		handler->startDocument();
		ch = is->get();
		while (ch != -1) 
		{
			if (ch == '<') 
			{
				skipWhiteSpace();
				ch = is->get();
				parseElement();
			}
			else {
				ch = is->get();
			}
		}
		handler->endDocument();
		obj = handler->getObject();
	}
	catch (std::exception e) 
	{
		if (errorHandler != nullptr) 
		{
			errorHandler->error(e);
		}
		else 
		{
			handler->error(e);
		}
	}
	return obj;
}

void XMLReader::parseElement()
{

	std::string empty;
	std::string name;
	std::string qname;
	std::string uri;
	std::string auri;
	std::string tag;
	std::string raw;
	std::string value;

	attrs.clear();
	if (ch == '/') 
	{
		ch = is->get();
		qname = getWord();
		nSize = 0;
		parseName(qname);
		name = tname;
		uri = turi;
		tname.clear();
		turi.clear();
		skipUntil('>');
		handler->endElement(uri, name, qname);
	}
	else 
	{
		qname = getWord();
		nSize = 0;
		parseName(qname);
		name = tname;
		uri = turi;
		tname.clear();
		turi.clear();
		skipWhiteSpace();

		if (ch != -1 && ch != '>' && ch != '/')
		{
			do 
			{
				append((char)ch);
				ch = is->get();
				if (ch == '=') 
				{
					raw = getBuffer();
					nSize = 0;
					parseName(raw);
					tag = tname;
					auri = turi;
					tname.clear();
					turi.clear();
					skipUntil('"');
					ch = is->get();

					while (ch != -1 && ch != '>') {
						if (ch == '"') {
							ch = is->get();
							value = getBuffer();

							if (!auri.empty() &&
								(auri=="xmlns" 
								|| auri=="xsi")) 
							{
								namespaces.insert(std::make_pair(tag, value));
							}
							else 
							{
								if (!auri.empty()) 
								{
									auto it = namespaces.find(auri);
									if (it != namespaces.end())
										auri = (*it).second;
								}
								attrs.addAttribute(auri, tag, raw, "", normalize(value));
							}
							nSize = 0;
							break;
						}
						append((char)ch);
						ch = is->get();
					}
					skipWhiteSpace();
				}
			} while (ch != -1 && ch != '>' && ch != '/');
		}

		if (!uri.empty())
		{
			auto it = namespaces.find(uri);
			if (it != namespaces.end())
				uri = (*it).second;
		}

		if (isPI(name)) 
		{
			handler->processingInstruction(name, empty);
		}
		else 
		{
			handler->startElement(uri, name, qname, attrs);
		}

		if (ch == '/') 
		{
			skipUntil('>');
			handler->endElement(uri, name, qname);
			return;
		}

		nSize = 0;
		if (ch == '>') 
		{
			ch = is->get();
			while (ch != -1 && ch != '<') 
			{
				append((char)ch);
				ch = is->get();
			}
		}

		if (nSize > 0) 
		{
			handler->characters(buffer, 0, nSize);
		}
	}
}

void XMLReader::parseName(const std::string & name)
{

	int colon = name.find_first_of(':');
	switch (colon) 
	{
	case -1:
		turi.clear();
		tname = name;
		break;

	case 0:
		throw new std::exception("error name can not start with a colon");

	default:
		turi = name.substr(0, colon);
		tname = name.substr(colon + 1);
		break;
	}
}

void XMLReader::normalize(std::string & chars, int start, int len)
{

	int j = start;
	for (int i = 0; i < len; i++, j++) 
	{
		if (chars[i] == '&') 
		{
			if (chars[i + 1] == 'l' && chars[i + 2] == 't'
				&& chars[i + 3] == ';') 
			{
				chars[j] = '<';
				i += 3;
			}
			else if (chars[i + 1] == 'g' && chars[i + 2] == 't'
				&& chars[i + 3] == ';') 
			{
				chars[j] = '>';
				i += 3;
			}
			else if (chars[i + 1] == 'a' && chars[i + 2] == 'm'
				&& chars[i + 3] == 'p' && chars[i + 4] == ';') 
			{
				chars[j] = '&';
				i += 4;
			}
			else if (chars[i + 1] == 'q' && chars[i + 2] == 'u'
				&& chars[i + 3] == 'o' && chars[i + 4] == 't'
				&& chars[i + 5] == ';') 
			{
				chars[j] = '"';
				i += 5;
			}
			else if ((chars[i + 1] == '#' && chars[i + 3] == ';')
				&& (chars[i + 2] == '\r' || chars[i + 2] == '\n')) 
			{
				chars[j] = chars[i + 2];
				i += 3;
			}
			else 
			{
				chars[j] = chars[i];
			}
		}
		else 
		{
			chars[j] = chars[i];
		}
	}
	
}



std::string XMLReader::normalize(std::string & value)
{
	if (value.empty() || value.find_first_of('&') == -1) 
	{
		return value;
	}
	
	normalize(value, 0, value.length());
	return value;
}

bool XMLReader::isPI(const std::string & chars)
{
	for (auto & c : chars) 
	{
		if (c == '?') 
		{
			return true;
		}
		else if (c != '<' && c != ' ') 
		{
			break;
		}
	}
	return false;
}

void main(char * args[])
{
	try 
	{
		XMLReader xr;
		std::ifstream findFile(args[1], std::ifstream::in);
		std::shared_ptr<DefaultHandler> handler(new DefaultHandler());
		xr.setContentHandler(handler.get());
		xr.parse(&findFile);
	}
	catch (std::exception e) 
	{
	}
}

