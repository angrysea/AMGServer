#pragma once

#include <memory.h>
#include <string>

#include "Socket.h"

class BufferedSocketWriter
{
public:

	BufferedSocketWriter(Socket & s)
		:socket(s),
		 cursor(0),
		 len(Socket::max) 
	{
		buffer = new char[len];
	}

	virtual ~BufferedSocketWriter() 
	{
		delete [] buffer;
	}

	void write(char data) 
	{
		if(cursor+1>len) 
		{
			char * temp = new char[len+Socket::max];
			memcpy(temp,buffer,len);
			delete [] buffer;
			buffer=temp;
			len+=Socket::max;
		}

		buffer[cursor]=data;
		cursor++;
	}

	void write(int i) 
	{
		char data[128];
		if(!_itoa_s(i,data, sizeof(data), 10))
			write(data, (int)strlen(data));
	}

	void write(const char * data) 
	{
		write(data, (int)strlen(data));
	}

	void write(const std::string & data) 
	{
		write(data.c_str(), (int)data.length());
	}

	void write(const char * data, int size) 
	{
		if(cursor+size>len) 
		{
			char * temp = new char[len+Socket::max];
			memcpy(temp,buffer,len);
			delete [] buffer;
			buffer=temp;
			len+=Socket::max;
		}

		memcpy(buffer+cursor,data,size);
		cursor+=size;
	}

	int flush() 
	{
		int flushed = cursor;
		if(cursor>0) 
		{
			socket.send(buffer,&flushed);
			//delete [] buffer;
			//len=max;
			//buffer = new char[len];
			cursor = 0;
		}
		return flushed;
	}

private:
	char * buffer;
	int cursor;
	int len;
	Socket & socket;
};
