#pragma once

#include "Runnable.h"
#include "..\..\socket\socket\SocketTunnel.h"
#include "BaseHandler.h"
#include "HttpConnection.h"
#include "Request.h"

class HttpTunnel : public BaseHandler
{
public:
	HttpTunnel(Socket * clientSocket)
		:BaseHandler(clientSocket) 
	{
	}

	void process(Request * request) 
	{
		HttpConnection * connection =0;
		if(bUseProxy)
			connection = new HttpConnection(proxyServer,proxyPort);
		else
			connection = new HttpConnection(server,port);
		if(connection->openConnection()) 
		{
			Request wrapper;
			wrapper.setMethod("POST");
			std::string url = "http://";
			url.append(server);
			url.append(":");
			char temp[20];
			errno_t err = _itoa_s(port,temp,20,10);
			url.append(temp);
			url.append("/httproute");
			wrapper.setURI(url);
			wrapper.setProperty("Accept", "*/*");
			wrapper.setProperty("Accept-Language", "en-us");
			wrapper.setProperty("User-Agent", "Mozilla/4.0 (Compatible)");
			err = _itoa_s(request->getSize(),temp,20,10);
			wrapper.setProperty("Content-Length",temp);
			wrapper.setProperty("Content-Type", "text/html");
			if(bUseProxy&&authorization!=0)
				wrapper.setAuthorization(authorization);

			BufferedSocketWriter * writer = connection->getWriter();
			wrapper.post(writer,false);
			request->post(writer);

			Socket * socket = getClientSocket();
			Socket * reader = connection->getSocket();
			
			char data[Socket::max];
			int l=0;
			do 
			{
				l = Socket::max;
				if(reader->receive(data, &l)!=false) 
				{
					if(l>0)
						socket->send(data,&l);
				}
			} while(l>0);

			socket->shutDown(SD_BOTH);
			socket->close();
			reader->shutDown(SD_RECEIVE);
			reader->close();
		}
		delete connection;
	}
};
