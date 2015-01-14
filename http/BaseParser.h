#pragma once

#include "BufferedReader.h"
#include "BufferedSocketWriter.h"
#include "BufferedSocketReader.h"
#include "BufferedStringReader.h"

class BaseParser
{
public:
	
	BaseParser(Socket & s)
		:ch(BufferedReader::eof),
		 nSize(0) 
	{
		reader = new BufferedSocketReader(s);
		buffer = new unsigned char[2048];
		bufferlength=2048;
	}

	BaseParser(const std::string & s)
		:ch(BufferedReader::eof),
		 nSize(0) 
	{
		reader = new BufferedStringReader(s);
		buffer = new unsigned char[2048];
		bufferlength=2048;
	}

	BaseParser()
		:ch(BufferedReader::eof),
		nSize(0),
		reader(0) 
	{
		buffer = new unsigned char[2048];
		bufferlength=2048;
	}

	~BaseParser() 
	{
		delete [] buffer;
		delete reader;
	}

	void reset() {
		nSize=0;
	}

	void setStream(Socket & s) 
	{
		reader = new BufferedSocketReader(s);
	}

    std::string getWord() 
	{
		nSize=0;
		std::string ret;

		while(ch != ' ' && ch != '/' && ch!='>' && ch >= 32) 
		{
			append((char)ch);
			ch=reader->get();
		}

		if(nSize>0) 
		{
			ret=getBuffer();
		}

		return ret;
	}

	std::string getValue(int endChar, bool inclusive) 
	{
		std::string ret;
		nSize=0;

		if(inclusive==false) 
		{
			ch=reader->get();
		}

		while(ch!=endChar && ch != BufferedReader::eof) 
		{
			append((char)ch);
			ch=reader->get();
		}

		if(inclusive==true) 
		{
			append((char)ch);
		}

		if(nSize>0) 
		{
			ret=getBuffer();
		}
		ch=reader->get();
		return ret;
	}

	std::string getToken(int endChar) 
	{
		std::string ret;
		nSize=0;

		ch=reader->get();

		while(ch!=endChar && ch != BufferedReader::eof) 
		{
			append((char)ch);
			ch=reader->get();
		}

		if(nSize>0) 
		{
			ret=getBuffer();
		}

		return ret;
	}

	void skipUntil(int i) 
	{
		while((ch!=BufferedReader::eof) && (ch!=i)) 
		{
			ch=reader->get();
		}	
	}

	void skipUntilWhiteSpace() 
	{
		while ((ch!=BufferedReader::eof) && (ch!=' ') && (ch!='\t')) 
		{
			ch=reader->get();
		}
	}

	void skipWhiteSpace() 
	{
		while ((ch==' ') || (ch == '\t') || (ch == '\r') || (ch == '\n')) 
		{
			ch=reader->get();
		}
	}

	std::string getBuffer() 
	{
		return std::string((const char*)buffer,nSize);
	}

	void append(int c) 
	{
		if(nSize+1>=bufferlength) 
		{
			int nlength = bufferlength*2;
			unsigned char * newBuffer =new unsigned char[nlength] ;
			memset(newBuffer,0x00,nlength);
			memcpy(newBuffer,buffer,bufferlength);
			buffer=newBuffer;
			bufferlength=nlength;
		}
		buffer[nSize++]=(unsigned char)c;
	}

protected:
	int ch;
	BufferedReader * reader;
    unsigned char * buffer;
	int bufferlength;
    int nSize;
};
