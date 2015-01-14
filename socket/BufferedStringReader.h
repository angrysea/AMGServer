#pragma once

#include "bufferedreader.h"

class BufferedStringReader : public BufferedReader
{
public:

	BufferedStringReader(const std::string & data) 
		:cursor(eof) 
	{
		len=(int)data.length();
		buffer = new char[len];
		memcpy(buffer,data.c_str(),len);
	}

	virtual ~BufferedStringReader() 
	{
		delete [] buffer;
	}

	virtual char get() 
	{

		if(++cursor<len) 
		{
			return buffer[cursor];
		}
		return eof;
	}

	virtual char * get(int & l) 
	{

		char * ret = 0;
		if(++cursor<len) 
		{
			int i = len-cursor > l ? len-cursor : l;
			l=i;
			ret=&buffer[cursor];
			cursor+=l;
		}
		else 
		{
			l=eof;
		}
		return ret;
	}

private:
	char * buffer;
	int cursor;
	int len;
};
