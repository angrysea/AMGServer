// Socket.h: interface for the Socket class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <winsock2.h>
#include <atlbase.h>
#include "network_io.h"

class Socket 
{
public:
	static const int max=102400;

	Socket();
	virtual ~Socket();

public:
	bool create(int type);
	bool create();
	bool cleanup();
	bool shutDown(int how=SD_BOTH);
	bool close();
	bool bind();
	bool listen(int backlog);
	bool accept(Socket& connectedSocket);
	bool connect(const char * hostname, int port);
	bool receive(char *buf, int *len);
	bool send(const char *buf);
	bool send(const char *buf, int *len);

	bool setblockoff();
	bool setblockon();
	bool setKeepAlive();
	bool setsocketopt(int opt, int on);
	bool getsocketopt(int opt, int & on);
	bool gethostname(char *buf, int len);
	bool getremotehostname(std::string & name);

	operator SOCKET() const 
	{ 
		return sock; 
	}
	
	bool invalid() 
	{
		return sock==INVALID_SOCKET;
	}

	Socket & operator =(SOCKET s) 
	{ 
		sock = s;
		return *this; 
	}
	
	bool getlocaladdr() 
	{		
		int namelen = sizeof(local_addr);

		if (getsockname(sock, (sockaddr *)&local_addr, &namelen) < 0) 
		{
			return false;
		}
		else 
		{
			local_port_unknown = local_interface_unknown = 0;
			return true;
		}
	}

	bool setlocalport(unsigned int port) 
	{
		local_addr.sin_port = htons((short)port);
		return true;
	}

	bool setremoteport(unsigned int port) 
	{
		remote_addr.sin_port = htons((short)port);
		return true;
	}

	bool getlocalport(unsigned int *port, SOCKET *sock) 
	{		
		if (local_port_unknown) 
		{
			bool rv = getlocaladdr()==0?false:true;

			if (rv != true) 
			{
				return rv;
			}
		}

		*port = ntohs(local_addr.sin_port);
		return true;
	}

	bool getremoteport(unsigned int *port, SOCKET *sock) 
	{
		*port = ntohs(remote_addr.sin_port);
		return true;
	}

	bool set_local_ipaddr(const char *addr) 
	{		
		unsigned long ipaddr;
    
		if (!strcmp(addr, AMG_ANYADDR)) 
		{
			local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			return true;
		}
    
		ipaddr = inet_addr(addr);
    
		if (ipaddr == INADDR_NONE) 
		{
			return false;
		}
    
		local_addr.sin_addr.s_addr = ipaddr;
		return true;
	}

	bool setremoteipaddr(const char  *addr) 
	{
		unsigned long ipaddr;
    
		if (!strcmp(addr, AMG_ANYADDR)) 
		{
			remote_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			return true;
		}
    
		ipaddr = inet_addr(addr);
    
		if (ipaddr == INADDR_NONE) 
		{
			return false;
		}
    
		remote_addr.sin_addr.s_addr = ipaddr;
		return true;
	}

	bool getlocalipaddr(char **addr, SOCKET *sock) 
	{		
		if (local_interface_unknown) 
		{
			bool rv = getlocaladdr()==0?false:true;

			if (rv != true) 
			{
				return rv;
			}
		}

		*addr=_strdup(inet_ntoa(local_addr.sin_addr));
		return true;
	}

	bool getremoteipaddr(char  **addr) {
		*addr=_strdup(inet_ntoa(remote_addr.sin_addr));
		return true;
	}

	bool getlocalname(sockaddr_in **name) 
	{
		if (local_port_unknown || local_interface_unknown) 
		{
			bool rv = getlocaladdr();

			if (rv != true) 
			{
				return rv;
			}
		}
		*name = &local_addr;
		return true;
	}

	bool getremotename(sockaddr_in **name, SOCKET *sock) 
	{
		*name = &remote_addr;
		return true;
	}

    sockaddr_in * get_localaddr() 
	{
		return &local_addr;
	}

    sockaddr_in * get_remoteaddr() 
	{
		return &remote_addr;
	}

    void set_localaddr(sockaddr_in * addr) 
	{
		local_addr = *addr;
	}

    void set_remoteaddr(sockaddr_in * addr) 
	{
		remote_addr = *addr;
	}

    int get_addrlen() 
	{
		return addr_len;
	}

    void set_addrlen(int len) 
	{
		addr_len=len;
	}

    void settimeout(long to) 
	{
		timeout = to;
	}

    long gettimeout() 
	{
		return timeout;
	}

    unsigned long getdisconnected() 
	{
		return disconnected;
	}

    void setdisconnected(unsigned long d) 
	{
		disconnected=d;
	}

	void setSOCKET(SOCKET s) 
	{
		sock=s;
	}

    void set_local_port_unknown(int n) 
	{
		local_port_unknown=n;
	}

    void set_local_interface_unknown(int n) 
	{
		local_interface_unknown=n;
	}

	int getLastError() 
	{
		return WSAGetLastError();
	}

	int setKeepAlive(bool alive) 
	{
		return ::setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char *)&alive, sizeof(bool));
	}

	int setRecBufferSize(int size) 
	{
		return ::setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(int));
	}

	int setSendBufferSize(int size) 
	{
		return ::setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(int));
	}

	int setRecvTimeOut(int time) 
	{
		return ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &time, sizeof(int));
	}

	int setSendTimeOut(int time) 
	{
		return ::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &time, sizeof(int));
	}


public:
	static bool socketInit( WSADATA* lpwsaData );

protected:
	SOCKET sock;
    sockaddr_in local_addr;
    sockaddr_in remote_addr;
    int addr_len;
    long timeout;
    unsigned long disconnected;
    int local_port_unknown;
    int local_interface_unknown;
};

