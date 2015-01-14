#pragma once
#include <string>
#include <sstream>
#include <map>

using namespace std;

#include "BufferedSocketWriter.h"
#include "BufferedSocketReader.h"

class HttpUrlConnection {
public:

	HttpUrlConnection(const string & url) :
		connected(false),
		responseCode(-1),
		method("GET"),
		useProxy(false) {
		setURL(url);
	}

	void shutdownOutput() {
		socket->shutDown();
	}

	void setProxy(const string & host, const string & port) {
		useProxy = true;
		proxyHost = host;
		proxyPort = port;
	}

	void openConnection() {

		connected = true;
		socket = new Socket();
		socket->create();
		socket->setsocketopt(AMG_SO_LINGER,0);
		socket->connect(host.c_str(), port);

		BufferedSocketWriter bsw(*socket);

		bsw.write(method);
		bsw.write(' ');
		bsw.write(uri);
		bsw.write(VERSION);
		bsw.write(USERAGENT);
		bsw.write(HOST);
		bsw.write(host);
		bsw.write(NEWLINE);
		bsw.write(ACCEPT);

		for(auto it = headerFields.begin(); it!=headerFields.end(); it++) {
			string key = (*it).first;
			string value = (*it).second;
			bsw.write(key);
			bsw.write(':');
			bsw.write(value);
			bsw.write(NEWLINE);
		}

		bsw.write(NEWLINE);
		bsw.flush();
	}

	const string & setRequestMethod(const string & newValue) {
		if (!connected) {
			method = newValue;
		}
		return method;
	}

	const string & getRequestMethod() const {
		return method;
	}

	int getResponseCode() const {
		return responseCode;
	}

	const string & getResponseMessage() const {
		return responseMessage;
	}

	void setContentLength(int len) {
		std::ostringstream stm;
		stm << len;
		setHeaderField("content-length", stm.str());
	}

	int getContentLength() {
		return getHeaderFieldInt("content-length", -1);
	}

	string getContentType() {
		return getHeaderField("content-type");
	}

	void setContentType(string type) {
		setHeaderField("content-type", type);
	}

	string getContentEncoding() {
		return getHeaderField("content-encoding");
	}

	long getExpiration() {
		return getHeaderFieldDate("expires", 0);
	}

	long getDate() {
		return getHeaderFieldDate("date", 0);
	}

	long getLastModified() {
		return getHeaderFieldDate("last-modified", 0);
	}

	map<const string, string> getHeaderFields() {
		return headerFields;
	}

	int getHeaderFieldInt(string name, int Default) {
		if(name.length()>0) {
			string value = getHeaderField(name);
			if(value.length()>0) {
				return std::atoi(value.c_str());
			}
		}
		return Default;
	}

	long getHeaderFieldDate(string name, long Default) {
		string datestring = getHeaderField(name);
		/*
			datestring.trim();
			if (datestring.indexOf("GMT") == -1) {
				datestring = datestring + " GMT";
			}
			return new SimpleDateFormat("EEE, dd MMM yyyy hh:mm:ss z").parse(
					datestring).getTime();
		*/
		return Default;
	}

	const string & getHeaderField(const string & name) {
		return (*(headerFields.find(name))).second;
	}

	void setHeaderField(const string & name, string & value) {
		headerFields.insert(std::pair<const string, string>(name, value));
	}

	void setURL(const string & url) 
	{
        size_t start = 0;
		uri=url;


		if(url.find("https://")==0) 
		{
            start=8;
        }
        else 
		{
            if(url.find("http://")==0) 
			{
                start = 7;
            }
        }

		size_t colon = (unsigned int)url.find(":",start);
		size_t slash = (unsigned int)url.find("/",start);
		
		if(colon!=std::string::npos)
		{
			if(slash==std::string::npos)
			{
				host = url.substr(start,colon-start);
				port=atoi(url.substr(colon+1).c_str());
			}
			else if(colon>slash)
			{
				host = url.substr(start,slash-start);
				port=80;
			}
			else 
			{
				host = url.substr(start,colon-start);
				port=atoi(url.substr(colon+1,slash-colon).c_str());
			}
		}
		else 
		{ 
			port=80;
			if(slash==std::string::npos)
			{
				host = url.substr(start);
			}
			else 
			{
				host = url.substr(start,slash-start);
			}
		}

        if(slash!=std::string::npos && slash+1<url.length()) {
            start=slash+1;
            path=url.substr(start);
        }
    }


	BufferedSocketWriter getOutputStream() {
		return BufferedSocketWriter(*socket);
	}

	BufferedSocketReader getInputStream() {
		return BufferedSocketReader(*socket);
	}

	void setKeepAlive(boolean b) {
		//socket->setKeepAlive(b);
	}

	void setReceiveBufferSize(int size) {
		//socket->setReceiveBufferSize(size);
	}

protected:
	int responseCode;
	string responseMessage;

private:
	Socket * socket;
	string host;
	int port;
	string path;
	string method;
	string uri;
	bool connected;
	map<const string, string> headerFields;
	bool useProxy;
	string proxyHost;
	string proxyPort;

	static const string methods[];
	static const string VERSION;
	static const string ACCEPT;
	static const string USERAGENT;
	static const string HOST;
	static const string NEWLINE;
};
