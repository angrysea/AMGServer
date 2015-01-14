// Socket.cpp: implementation of the Socket class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "Socket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Socket::Socket()
		  :sock( INVALID_SOCKET) {
    memset(&local_addr, 0, sizeof(sockaddr_in));
    memset(&remote_addr, 0, sizeof(sockaddr_in));
}

Socket::~Socket() {

	if ( sock!=INVALID_SOCKET ) {
		close();
	}
}

bool Socket::socketInit(WSADATA* lpwsaData) {

	bool bRet = true;
	WSADATA wsaData;

	try {
		if(lpwsaData==0) {
			lpwsaData = &wsaData;

			WORD wVersionRequested = MAKEWORD(1, 1);
			int nResult = WSAStartup( wVersionRequested, lpwsaData);
			if (nResult!=0) {
			   throw false;
			}
			if (LOBYTE(lpwsaData->wVersion)!=1 || HIBYTE(lpwsaData->wVersion)!=1) {
				WSACleanup();
				throw false;
			}
		}
	}
	catch(bool e) {
		bRet = e;
	}
	return bRet;
}

bool Socket::close() {
    
	if (sock!=INVALID_SOCKET)  {
		if(shutDown(SD_SEND)) {
			char junk[128];
			int read = 0;
			while(true) {
				read =128;
				read=receive(junk, &read);
				if(read==SOCKET_ERROR) {
					// Must have been shutdown on the other side;
					break;
				}
				else if(read>127)
					std::cerr<< "Unexpected bytes during socket close " << std::endl <<
						"Data : "	<< junk << " for size = " << read << std::endl;
				else 
					break;
			}
		}		

		shutDown(SD_RECEIVE);
        if (::closesocket(sock)==SOCKET_ERROR) {
            return false;
        }
        sock = INVALID_SOCKET;
    }
    return true;
}

bool Socket::shutDown(int how) { 
	return (SOCKET_ERROR!=::shutdown(sock,how)); 
}

bool Socket::create() {
	return create(AF_INET);
}

bool Socket::create(int type) {

	sock = ::socket(type, SOCK_STREAM, IPPROTO_TCP);
    if(sock == INVALID_SOCKET)  {
        return false;
    }

    local_addr.sin_family = type;
    remote_addr.sin_family = type;
    addr_len = sizeof(local_addr);
    local_addr.sin_port = 0;   
    timeout = -1;
    disconnected = 0;

	return true;
}

bool Socket::bind() {

    if(::bind(sock, (sockaddr *)&local_addr, addr_len)==-1) {
        return false;
    }
    else if(local_addr.sin_port == 0) {
        local_port_unknown = 1;
    }
	return true;
}

bool Socket::listen(int backlog) {

    if (::listen(sock, backlog)==SOCKET_ERROR)
        return false;
    else
        return true;
}

bool Socket::accept(Socket & connectedSocket) {

    memcpy(&(connectedSocket.local_addr), &local_addr, sizeof(sockaddr_in));
    memcpy(&(connectedSocket.remote_addr), &remote_addr, sizeof(sockaddr_in));

    connectedSocket.set_addrlen(sizeof(sockaddr_in));
    connectedSocket.settimeout(-1);
    connectedSocket.setdisconnected(0);

	int len = connectedSocket.get_addrlen();
    connectedSocket = ::accept(sock, (sockaddr *)connectedSocket.get_localaddr(), &len);

	connectedSocket.set_addrlen(len);
	connectedSocket.set_localaddr(&local_addr);

    if(local_port_unknown) {
        connectedSocket.set_local_port_unknown(1);
    }

    if (local_interface_unknown||local_addr.sin_addr.s_addr == 0) {
        connectedSocket.set_local_interface_unknown(1);
    }

    return true;
}

bool Socket::connect(const char * hostname, int port) {

    hostent *hp;
    int lasterror;
    fd_set temp;

    if (hostname!=0) {
        if (*hostname >= '0' && *hostname <= '9' && 
			strspn(hostname, "0123456789.") == strlen(hostname)) {
            remote_addr.sin_addr.s_addr = inet_addr(hostname);
        }
        else {
            hp = gethostbyname(hostname);
            if(hp==0) {
                return false;
            }
            memcpy((void *)&remote_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
            addr_len = sizeof(remote_addr);
        }
    }
    
	setremoteport(port);
    remote_addr.sin_family = AF_INET;

    if (::connect(sock, (const sockaddr *)&remote_addr, addr_len) == SOCKET_ERROR) {
        lasterror = WSAGetLastError();
        if (lasterror!=WSAEWOULDBLOCK) {
            return false;
        }

        FD_ZERO(&temp);
        FD_SET(sock, &temp);
        if (::select(sock+1, 0, &temp, 0, 0) == SOCKET_ERROR) {
            return false;
        }
    }

    if (local_addr.sin_port==0) {
        local_port_unknown = 1;
    }

    if (local_addr.sin_addr.s_addr == 0) {
        local_interface_unknown = 1;
    }

    return true;
}

bool Socket::setblockoff() {

	unsigned long zero = 0;

    if (ioctlsocket(sock, FIONBIO, &zero) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool Socket::setblockon() {

	unsigned long one = 1;

    if (ioctlsocket(sock, FIONBIO, &one) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool Socket::setsocketopt(int opt, int on) {

	int one;
    bool stat;

    one = on ? 1 : 0;

    switch (opt) {
		case AMG_SO_TIMEOUT: {
			if (on > 0) {
				on = on/1000;
			}

			if (on == 0) {
				if (timeout!=0) {
					if ((stat = setblockon())!=true)
						return stat;
				}
			}
			else if (on > 0) {
				if (timeout == 0) {
					if ((stat = setblockoff())!=true)
						return stat;
				}
				if (timeout!=on) 
				{
					::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &on, sizeof(on));
					::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &on, sizeof(on));
				}
			}
			else if (on < 0) {
				int zero = 0;
				if ((stat = setblockoff())!=true)
					return stat;
				::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &zero, sizeof(zero));
				::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &zero, sizeof(zero));
			}
			timeout = on;
			break;
		}
	    
		case AMG_SO_KEEPALIVE:
			if (::setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char *)&one, sizeof(int)) == -1) {
				return false;
			}
			break;
	    
		case AMG_SO_DEBUG:
			if (::setsockopt(sock, SOL_SOCKET, SO_DEBUG, (const char *)&one, sizeof(int)) == -1) {
				return false;
			}
			break;
	    
		case AMG_SO_REUSEADDR:
			if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(int)) == -1) {
				return false;
			}
			break;

		case AMG_SO_NONBLOCK:
			if (on) {
				if ((stat = setblockon())!=true) 
					return stat;
			}
			else {
				if ((stat = setblockoff())!=true)
					return stat;
			}
			break;

		case AMG_SO_LINGER: {
			struct linger li;
			li.l_onoff = on;
			if(on==0)
				li.l_linger = 0;
			else
				li.l_linger = MAX_SECS_TO_LINGER;
			if (::setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &li, sizeof(struct linger)) == -1) {
				return false;
			}
			break;
		}
		default:
			return false;
			break;
    }

    return true;
}

bool Socket::getsocketopt(int opt, int & on) {

    switch (opt) {
		case AMG_SO_TIMEOUT: 
			on = timeout * 1000;
			break;

		case AMG_SO_DISCONNECTED:
			on = disconnected;
			break;

		case AMG_SO_KEEPALIVE:
		case AMG_SO_DEBUG:
		case AMG_SO_REUSEADDR:
		case AMG_SO_NONBLOCK:
		case AMG_SO_LINGER:
		default:
			return false;
			break;
    }
    return true;
}


bool Socket::gethostname(char *buf, int len) {

	if (::gethostname(buf, len) == -1)
        return false;
    else
        return true;
}

bool Socket::getremotehostname(std::string & name) {

    hostent *hptr;

    hptr = gethostbyaddr((char *)&(local_addr.sin_addr),sizeof(in_addr), AF_INET);

    if (hptr!=0) {
        name = hptr->h_name;
        return true;
    }
    return false;
}

bool Socket::send(const char *buf) {
	int len = strlen(buf);
	return send(buf,&len);
}

bool Socket::send(const char *buf, int *len) {

	int rv;

    rv = ::send(sock, buf, *len, 0);
    *len = rv;
    if (rv == SOCKET_ERROR) {
        return false;
    }

    return true;
}

bool Socket::receive(char *buf, int *len) {

	int rv;
    DWORD flags = 0;

    rv = recv(sock, buf, *len, flags);
	*len = rv;
    if (rv == SOCKET_ERROR) {
        return false;
    }

    return true;

}
