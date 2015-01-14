// server.cpp : Defines the entry point for the console application.
//
#include <functional>
#include <exception>

#include "IServer.h"

std::shared_ptr<IServer> IServer::server = nullptr;
std::string IServer::key;
std::mutex IServer::mx;

IServer::IServer()
{
	if (server == nullptr)
	{
		std::lock_guard<std::mutex> lg(mx);
		if (server == nullptr)
		{
			server = this;
		}
		else
		{
			throw new std::exception("Can not instantiate second instance of IServer");
		}
	}
	else
	{
		throw new std::exception("Can not instantiate second instance of IServer");
	}
}

IServer::~IServer()
{

}

std::shared_ptr<IServer> & IServer::getserver()
{
	return server;
}

void * IServer::getNamedService(std::string name)
{
	return server->getService(name);
}

std::string IServer::getKey()
{
	return key;
}

void IServer::setKey()
{
	try
	{
		std::hash<std::string> hash_fn;
		key = std::to_string(hash_fn(server->getHost() + std::to_string(server->getPort())));
	}
	catch (std::exception e)
	{
	}
}

const std::string & IServer::getCertificate()
{
	return "";
}

void * IServer::getController(std::string name)
{
	return server->getAvailableController(name);
}

