#pragma once

#include "BufferedSocketWriter.h"
#include "BufferedSocketReader.h"

class HttpConnection
{
public:
    HttpConnection(const std::string & url) 
		:connected(false),
		 socket(0) 
	{
		setURL(url);
    }

    HttpConnection(const std::string & host, int port) 
		:connected(false),
		 socket(0),
		 host(host),
		 port(port) 
	{
    }

	virtual ~HttpConnection(void) 
	{
		delete socket;
	}

	bool openConnection() 
	{
		connected=true;
		socket = new Socket();
		socket->create();
		//socket->setRecBufferSize(Socket::max);
		//socket->setSendBufferSize(Socket::max);
		//socket->settimeout(-1);
		//socket->setdisconnected(0);
		socket->setsocketopt(AMG_SO_LINGER,0);
		return socket->connect(host.c_str(), port);
	}

	void close() 
	{
		socket->close();
	}

	void setURL(const std::string & url) 
	{
        size_t start = 0;
		uri=url;


		if(url.find("https://")==0) 
		{
            start=8;
            preffix = "https";
        }
        else 
		{
            preffix="http";
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

	Socket * getSocket() 
	{
		return socket;
	}

    BufferedSocketReader * getReader() 
	{
	    return new BufferedSocketReader(*socket);
    }

    BufferedSocketWriter * getWriter() 
	{
	    return new BufferedSocketWriter(*socket);
    }

private:
	Socket * socket;
    std::string preffix;
    std::string host;
    int port;
    std::string uri;
    std::string path;
	bool connected;
};
