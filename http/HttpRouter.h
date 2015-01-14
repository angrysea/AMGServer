#pragma once

#include "Runnable.h"
#include "Socket.h"
#include "SocketTunnel.h"

#include "HttpParser.h"
#include "BaseHandler.h"
#include "HttpConnection.h"
#include "Request.h"

class HttpRouter : public BaseHandler 
{
public:
	HttpRouter(Socket * clientSocket)
		:BaseHandler(clientSocket) 
	{
	}

	void process(Request * request) 
	{
		HttpConnection * connection = 0;
		Request * tofoward = HttpParser(request->getRequest(), httpType).parse();
		if(bUseProxy)
			connection = new HttpConnection(proxyServer,proxyPort);
		else 
			connection = new HttpConnection(tofoward->getURI());
		
		if(connection->openConnection()) 
		{
			tofoward->setAuthorization(authorization);
			tofoward->post(connection->getWriter());

			Socket * socket = getClientSocket();
			Socket * reader = connection->getSocket();

			int l = 0;
			char data[Socket::max];
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
		delete tofoward;
	}
};
