#pragma once

#include <string.h>
#include <iostream>
#include "BaseHandler.h"
#include "Request.h"
#include "PortRouter.h"
#include "HttpRouter.h"

class Router : public BaseHandler 
{
public:
	Router(Socket * clientSocket)
		:BaseHandler(clientSocket) 
	{
	}

	virtual ~Router() 
	{
		delete handler;
	}

	void process(Request * request) 
	{
		const std::string & uri = request->getURI();
		size_t start = uri.find_last_of("/");
		std::string command = uri.substr(start+1);
		if(command=="httproute") 
		{
			handler = new HttpRouter(getClientSocket());
			if(bUseProxy) 
			{
				handler->setAuthorization(authorization);
				handler->setProxyServer(proxyServer);
				handler->setProxyPort(proxyPort);
			}
			handler->process(request);
		}
		else if(command=="portroute") 
		{
			handler = new PortRouter(getClientSocket());
			handler->process(request);
		}
	}

private:
	BaseHandler * handler;
};
