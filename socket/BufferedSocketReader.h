#pragma once

#include "Socket.h"
#include "BufferedReader.h"

class BufferedSocketReader : public BufferedReader
{
public:
	BufferedSocketReader(Socket & s)
		:BufferedReader(),
		 socket(s),
		 cursor(-1),
		 len(eof) {
	}

	char get() 
	{
		while(true) 
		{
			if(++cursor<len) 
			{
				return buffer[cursor];
			}
			if (len>0&&len<Socket::max) {
				return eof;
			}
			else
			{
				cursor=-1;
				len=Socket::max;
				memset(buffer, 0x00, Socket::max); 
				if(socket.receive(buffer, &len)==false) 
				{
					break;
				}
				if(len==0)
					return eof;
			}
		}
		return eof;
	}

	char * get(int & l) 
	{
		memset(buffer, 0x00, Socket::max); 
		if(socket.receive(buffer, &l)==false) 
		{
			l=eof;
		}
		cursor=-1;
		return buffer;
	}

private:
	char buffer[Socket::max+1];
	int cursor;
	int len;
	Socket & socket;
};
