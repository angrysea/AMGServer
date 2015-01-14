// ServerSocket.h: interface for the ServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Socket.h"

class ServerSocket : public Socket
{
public:
	ServerSocket();
	virtual ~ServerSocket();
	bool accept(Socket& connectedSocket);
};
