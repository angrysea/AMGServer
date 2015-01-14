#include "stdafx.h"

#include "ServerSocket.h"

ServerSocket::ServerSocket()
				:Socket() 
{
}

ServerSocket::~ServerSocket() 
{
}

bool ServerSocket::accept(Socket& connectedSocket) 
{
    memcpy(connectedSocket.get_localaddr(), &local_addr, sizeof(sockaddr_in));
    memcpy(connectedSocket.get_localaddr(), &remote_addr, sizeof(sockaddr_in));

    connectedSocket.set_addrlen(sizeof(sockaddr_in));
    connectedSocket.settimeout(-1);
    connectedSocket.setdisconnected(0);

	int len = connectedSocket.get_addrlen();
	do 
	{
		connectedSocket = ::accept(sock, (sockaddr *)connectedSocket.get_localaddr(), &len);
        Sleep(500);

	} while (connectedSocket.invalid());

    if (connectedSocket.invalid()) 
	{
        return false;
    }

	connectedSocket.set_addrlen(len);
	connectedSocket.set_localaddr(&local_addr);

    if(local_port_unknown) 
	{
        connectedSocket.set_local_port_unknown(1);
    }

    if (local_interface_unknown||local_addr.sin_addr.s_addr == 0) 
	{
        connectedSocket.set_local_interface_unknown(1);
    }

    return true;
}

