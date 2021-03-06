#pragma once

#include <string>
#include <map>

#include "..\..\socket\socket\BufferedSocketWriter.h"

class Page
{
public:	
	static const short NONE=0;
    static const short XML_TEXT=1;
    static const short HTML_TEXT=2;
    static const short HTML_IMAGE=3;
    static const short COMMAND=4;
    static const short SERVLET=5;

	static const char * VERSION;
	static const char * ACCEPT;
	static const char * USERAGENT;
	static const char * HOST;
	static const char * NEWLINE;
	static const char * empty;
    static const char * POST;
    static const char * GET;
	static const char * CONTENT_TYPE;
    static const char * methods[];


public:

	Page() :
		major(1),
		minor(1),
		mimeType(0),
		contentLength(0),
		lport(0) {
	}

	virtual ~Page() 
	{
	}

	const std::string & getHost() const 
	{
		getProperty(HOST);
	}

	void putProperty(std::string & name, std::string & value) 
	{
	    properties.insert(std::multimap<std::string,std::string>::value_type(name,value));
	}

	const std::string & getProperty(const std::string & name) const 
	{
		std::multimap<std::string,std::string>::const_iterator it = properties.find(name);
		return (*it).second;
	}

	void setProperty(const std::string & name, const std::string & data) 
	{
		properties.insert(std::multimap<std::string,std::string>::value_type(name,data));
	}

	std::multimap<std::string,std::string>::const_iterator begin() const 
	{
		return properties.begin();
	}

	std::multimap<std::string,std::string>::const_iterator end() const 
	{
		return properties.end();
	}

	const std::string & getPage() const 
	{
	    return request;
    }

    int getPort() const 
	{
        return lport;
    }

    void setPage(char * newValue, int len) 
	{
		request.clear();
		request.append(newValue,len);
		contentLength=len;
    }

    void setPage(std::string & newValue) 
	{
		request = newValue;
		contentLength=(int)request.length();
    }

    short getPageType() const {
        return mimeType;
    }

    void setVersion(const std::string & data) 
	{

		char * next_token = 0;
		char * version = 0;
		char * rescode = 0;
		version=strtok_s(const_cast<char*>(data.c_str()), " ", &next_token);
		if(version!=0)
		{
			if((rescode=strtok_s(0, " ", &next_token))!=0)
			{
					resultscode=atoi(rescode);
			}

			version+=5;

			if((version=strtok_s(version,".", &next_token))!=0)
			{
				major=atoi(version);
				if((version=strtok_s(0,".", &next_token))!=0) 
				{
					minor=atoi(version);
				}
			}
		}
	}

    const std::string & getURI() const 
	{
        return url;
    }

    void setURI(const std::string & newValue) 
	{
		url=newValue;
	}

    void setVersion(char * data) 
	{

		char * version;
		char * next_token;
		if((version=strtok_s(data, " ", &next_token))!=0)
		{
			if((version=strtok_s(0, ".", &next_token))!=0)
			{
				major=atoi(version);
				if((version=strtok_s(0,".", &next_token))!=0) 
				{
					minor=atoi(version);
				}
			}
		}
	}

	short majorVersion() const 
	{
        return major;
    }

	void setMajorVersion(short newValue) 
	{
        major=newValue;
    }

    short minorVersion() const 
	{
        return minor;
    }

	void setMinorVersion(short newValue) 
	{
        minor=newValue;
    }

    const std::string & getMethod() const 
	{
        return method;
    }

    void setMethod(const std::string & newValue) 
	{
        method=newValue;
    }

	void setAuthorization(const char * authorization) 
	{
		properties.insert(std::multimap<std::string,std::string>::value_type("Proxy-Authorization",authorization));
	}

    const std::string getQueryString() const 
	{
        try 
		{
            return url.substr(url.find('?') + 1);
        }
        catch(...) 
		{
        }
        return "";
    }

    const std::string getPageURI() const 
	{
        size_t sep = url.find('?');
        if (sep == -1)
            return url;
        return url.substr(0,sep);
    }

	int getContentLength() const 
	{
		return contentLength;
	}

	void post(BufferedSocketWriter * writer) const 
	{
		post(writer,true);
	}

	void post(BufferedSocketWriter * writer, bool autoFlush) const 
	{
		writer->write(method.c_str());
		writer->write(' ');
		writer->write(url.c_str());
		writer->write(VERSION);
		std::multimap<std::string,std::string>::const_iterator it = begin();

		while(it!=end()) 
		{
			writer->write((*it).first.c_str());
			writer->write(": ");
			writer->write((*it).second.c_str());
			writer->write(NEWLINE);
			it++;
		}

		writer->write(NEWLINE);
		if(contentLength>0)
			writer->write(request.c_str());
		if(autoFlush) 
		{
			writer->flush();
			delete writer;
		}
	}

	int getSize() const 
	{
		int size = 14;
		size += (int)method.length();
		size += (int)url.length();
		std::multimap<std::string,std::string>::const_iterator it = begin();

		while(it!=end()) 
		{
			size += (int)(*it).first.length();
			size += (int)(*it).second.length();
			size += 4;
			it++;
		}
		size += contentLength;
		return size;
	}

	std::multimap<std::string,std::string>::iterator getPropertyBegin() {
		return properties.begin();
	}

	std::multimap<std::string,std::string>::iterator getPropertyEnd() {
		return properties.end();
	}

	std::multimap<std::string,std::string> & getProperties() {
		return properties;
	}


private:
    std::string request;
    std::string url;
    std::string username;
    std::string password;
    std::string method;
	std::string type;
    short major;
    short minor;
    short mimeType;
	short resultscode;
    int lport;
	int contentLength;
    std::multimap<std::string,std::string> properties;
};
