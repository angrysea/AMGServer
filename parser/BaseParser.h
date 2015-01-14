#pragma once

#include <string>
#include <istream>
#include <memory>

#include "export.h"

class PARSER_API BaseParser
{
public:
	BaseParser() = default;


	BaseParser(std::istream * is) :
		is{ is }
	{
	}

	std::istream * getReader()
	{
		return is;
	}

	void setReader(std::istream * newValue)
	{
		is = newValue;
	}

protected:
	std::string getWord()
	{

		nSize = 0;
		std::string ret;

		while (ch != ' ' && ch != '/' && ch != '>' && ch >= 32) 
		{
			append((char) ch);
			ch = is->get();
		}

		if (nSize > 0) {
			ret = getBuffer();
		}

		return ret;
	}

	std::string getValue(int endChar, bool inclusive)
	{
		std::string ret;
		nSize = 0;

		if (inclusive == false) 
		{
			ch = is->get();
		}

		while (ch != endChar && ch != -1) 
		{
			append((char) ch);
			ch = is->get();
		}

		if (inclusive == true) 
		{
			append((char) ch);
		}

		if (nSize > 0) 
		{
			ret = getBuffer();
		}

		ch = is->get();

		return ret;
	}

	std::string getToken(int endChar)
	{

		std::string ret;
		nSize = 0;

		ch = is->get();

		while (ch != endChar && ch != -1) 
		{
			append((char) ch);
			ch = is->get();
		}

		if (nSize > 0) 
		{
			ret = getBuffer();
		}

		return ret;
	}

	void skipUntil(int i) 
	{
		while ((ch != -1) && (ch != i)) 
		{
			ch = is->get();
		}
	}

	void skipUntilWhiteSpace() 
	{
		while ((ch != -1) && (ch != ' ') && (ch != '\t')) 
		{
			ch = is->get();
		}
	}

	void skipWhiteSpace() 
	{
		while ((ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n')) 
		{
			ch = is->get();
		}
	}

	std::string getBuffer() 
	{
		return std::string(buffer, 0, nSize);
	}

	void append(int c) 
	{

		if (nSize + 1 >= buffersize)
		{
			int orgbuffersize = buffersize;
			buffersize *= 2;
			char * newBuffer = new char[buffersize];
			memset(newBuffer, 0x00, sizeof(char)*buffersize);
			memcpy(newBuffer, buffer, sizeof(char)*orgbuffersize);
			delete buffer;
			buffer = newBuffer;
		}
		buffer[nSize++] = (char) c;
	}

protected:
	std::istream * is;
	int ch = -1;
	int buffersize = 128;
	char * buffer = new char[buffersize];
	int nSize = 0;
};
