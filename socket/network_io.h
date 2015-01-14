#pragma once

const unsigned short MAX_SECS_TO_LINGER=30;
const unsigned short TLMAXHOSTLEN=256;
const char AMG_ANYADDR[] =  "0.0.0.0";

const unsigned short AMG_SO_LINGER=1;
const unsigned short AMG_SO_KEEPALIVE=2;
const unsigned short AMG_SO_DEBUG=4;
const unsigned short AMG_SO_NONBLOCK=8;
const unsigned short AMG_SO_REUSEADDR=16;
const unsigned short AMG_SO_TIMEOUT=32;
const unsigned short AMG_SO_SNDBUF=64;
const unsigned short AMG_SO_RCVBUF=128;
const unsigned short AMG_SO_DISCONNECTED=256;

const unsigned short AMG_POLLIN=0x001;
const unsigned short AMG_POLLPRI=0x002;
const unsigned short AMG_POLLOUT=0x004;
const unsigned short AMG_POLLERR=0x010;
const unsigned short AMG_POLLHUP=0x020;
const unsigned short AMG_POLLNVAL=0x040;


class iovec 
{
public:
    char* iov_base;
    int   iov_len;
};

