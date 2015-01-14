#pragma once

#include <string.h>
#include <iostream>

#include "Runnable.h"
#include "Socket.h"
#include "SocketTunnel.h"
#include "Http.h"
#include "HttpParser.h"
#include "BaseHandler.h"
#include "HttpConnection.h"
#include "Request.h"

class PortRouter : public BaseHandler 
{
public:
	PortRouter(Socket * clientSocket)
		:BaseHandler(clientSocket) 
	{
	}

	void process(Request * request) 
	{
		HttpConnection * connection = 0;
		const std::string & requestData = request->getRequest();
		size_t start = requestData.find(HTTP::crlf);

		if(start!=std::string::npos) 
		{
			connection = new HttpConnection(requestData.substr(13,start));			
			if(connection->openConnection()) 
			{
				Socket * connSocket = connection->getSocket();
				int write = requestData.length()-(start+2);
				if(write>0) 
				{
					connSocket->send(requestData.substr(start+2).c_str(), &write);
				}

				Socket * socket = getClientSocket();
				SocketTunnel from(connSocket,socket);
				SocketTunnel to(socket,connSocket);

				from.start();
				to.start();

				from.join();
				to.join();
			}
			delete connection;
		}
	}
};
