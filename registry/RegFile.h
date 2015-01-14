#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_BUFFER 1024

class RegFile
{
public:
	RegFile() :
		fh(0),
		bytesRead(0),
		pos(0),
		beof(false)
	{
	}

	bool openRead(std::string & fileName);
	bool openWrite(std::string & fileName);
	bool getLine(std::string & line);
	int writeLine(std::string & line);
	int blankLine();
	int close();

	bool eof()
	{
		return beof;
	}

private:
	int loadBuffer();
	bool getNext(char & c);

private:
	bool beof;
	int fh;
	int bytesRead;
	int pos;
	char wbuf[MAX_BUFFER];
};
