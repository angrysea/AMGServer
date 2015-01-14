#pragma once

#include "Socket.h"

class BufferedReader
{
public:

	static const int eof=-99;

	BufferedReader() 
	{
	}

	virtual ~BufferedReader() 
	{
	}

	virtual char get()=0;
	virtual char * get(int & l)=0;
};
