#pragma once

#include <atlenc.h>

#include "Runnable.h"
#include "request.h"

class BaseHandler : public Runnable
{
public:
	static const std::string httpType;

	BaseHandler(Socket * clientSocket);

	virtual ~BaseHandler() 
	{
	}

	virtual void run();

	virtual void process(Request * request) 
	{
		delete request;
	}

	Socket * getClientSocket() 
	{
		return clientSocket;
	}

	const char * getAuthorization() const 
	{
		return authorization;
	}

	bool isAuthorized() const 
	{
		return authorization!=0;
	}

	void setServer(const std::string & newValue) 
	{
		server = newValue;
	}

	void setPort(int newValue) 
	{
		port = newValue;
	}
	
	void setProxyServer(const std::string & newValue) 
	{
		proxyServer = newValue;
		bUseProxy=true;
	}

	void setProxyPort(int newValue) 
	{
		proxyPort = newValue;
	}
	
	void setAuthorization(char * auth) 
	{
		authorization=auth;
	}

	void setAuthorization(const std::string & user, const std::string & pwd) 
	{
		std::string auth = user;
		auth.append(":");
		auth.append(pwd);

		int s = (int)auth.length();
		int d = Base64EncodeGetRequiredLength(s);
		char * auth64 = new char[d];
		if(Base64Encode((const BYTE *)auth.c_str(),s,auth64,&d)==TRUE) 
		{
			authorization = new char[d+7];
			memset(authorization,0x00,d+7);
			memcpy(authorization,"Basic ",6);
			memcpy(authorization+6,auth64,d);
		}
	}

protected:
	std::string server; 
	int port;

	bool bUseProxy;
	std::string proxyServer; 
	int proxyPort;
	char * authorization;

private:
	Socket * clientSocket;
};
