#include "basehandler.h"
#include "httptunnel.h"
#include "HttpParser.h"
#include "Request.h"
#include "Response.h"

const std::string BaseHandler::httpType = "HTTP";

BaseHandler::BaseHandler(Socket * clientSocket)
	:clientSocket(clientSocket),
	 authorization(0),
	 bUseProxy(false)
{
}

void BaseHandler::run() 
{
	Request * request = HttpParser(*getClientSocket(), httpType).parse();
	if(authorization!=0)
		request->setAuthorization(authorization);
	
	process(request);
	
	delete request;
}
