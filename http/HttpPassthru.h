#pragma once

#include "Runnable.h"
#include "Socket.h"
#include "BufferedSocketWriter.h"
#include "BufferedSocketReader.h"
#include "BaseHandler.h"
#include "HttpConnection.h"
#include "Request.h"


class HttpPassthru : public BaseHandler 
{
public:

	HttpPassthru(Socket * clientSocket)
		:BaseHandler(clientSocket) 
	{
	}

	void process(Request * request) 
	{

		HttpConnection * connection = 0; 
		if(bUseProxy)
			connection = new HttpConnection(proxyServer,proxyPort);
		else
			connection = new HttpConnection(request->getURI());
		
		if(connection->openConnection()) 
		{
			request->setAuthorization(authorization);
			request->post(connection->getWriter());

			// Not going to send anything to the connection
			// so shutdown the send.
			//connection->shutdown(SD_SEND);

			Socket * socket = getClientSocket();
			BufferedSocketReader * reader = connection->getReader();
			BufferedSocketWriter * writer = new BufferedSocketWriter(*socket);
			
			char * data = 0;
			int l = Socket::max;
			do 
			{
				data = reader->get(l);
				if(l>0) 
				{
					writer->write(data,l);
					writer->flush();
				}
			} while(l>0);

			connection->close();
		}
		delete connection;
	}
};
