#pragma once

#include "../amgsockets/SocketTunnel.h"
#include "../threads/thread.h"
#include "basehandler.h"
#include "HttpConnection.h"
#include "Request.h"

class PortTunnel : public BaseHandler
{
public:

	PortTunnel(Socket * clientSocket)
		:BaseHandler(clientSocket) {
	}

	virtual ~PortTunnel() {
	}

	void run() {

		Socket * socket = getClientSocket();
		BufferedSocketReader * reader = new BufferedSocketReader(*socket);
		
		char * data = 0;
		std::string body("BoundServer: ");
		body.append(boundServer);
		body.append(Request::NEWLINE);
		int l = Socket::max;
		do {
			data = reader->get(l);
			if(l>0) {
				body.append(data,l);
			}
		} while(l>0);

		HttpConnection * connection = 0;
		if(bUseProxy)
			connection = new HttpConnection(proxyServer,proxyPort);
		else
			connection = new HttpConnection(server,port);

		if(connection->openConnection()) {
			Request wrapper;

			wrapper.setMethod("POST");
			std::string url = "http://";
			url.append(server);
			url.append(":");
			char temp[20];
			_itoa_s(port,temp,20,10);
			url.append(temp);
			url.append("/portroute");
			wrapper.setURI(url);
			wrapper.setProperty("Accept", "*/*");
			wrapper.setProperty("Accept-Language", "en-us");
			wrapper.setProperty("User-Agent", "Mozilla/4.0 (Compatible)");
			wrapper.setProperty("Content-Type", "text/html");
			_itoa_s(body.length(),temp,20,10);
			wrapper.setProperty("Content-Length", temp);
			if(bUseProxy&&authorization!=0)
				wrapper.setAuthorization(authorization);

			BufferedSocketWriter * writer = connection->getWriter();
			wrapper.post(writer,false);

			if(body.length())
				writer->write(body.c_str(),body.length());

			writer->flush();
			delete reader;
			delete writer;
			
			SocketTunnel from(connection->getSocket(),socket);
			SocketTunnel to(socket,connection->getSocket());

			from.start();
			to.start();

			from.join();
			to.join();
		}
		delete connection;
	}

	void setBoundServerInfo(const std::string & name, int port) {
		boundServer = name;
		boundServer.append(":");
		char temp[20];
		_itoa_s(port,temp,20,10);
		boundServer.append(temp);
	}

private:
	std::string boundServer;
};
