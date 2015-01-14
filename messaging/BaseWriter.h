#pragma once
#include <string>
#include <sstream>
#include <memory>

class BaseWriter
{
public:
	BaseWriter()
	{
	}

	BaseWriter(std::stringstream * os) : ostream{ std::make_unique<std::stringstream>(os) }
	{
		*ostream << "<?xml version=\"1.0\"?>";
	}

	BaseWriter(std::stringstream * os, bool bHeader) : ostream{ std::make_unique<std::stringstream>(os) }
	{
		if (bHeader) 
		{
			*ostream << "<?xml version=\"1.0\"?>";
		}
	}

	void writeString(std::string data)  
	{
		*ostream << data;
	}

	 void writeLine(const std::string & data)  
	 {
		 for (auto c : data)
		 {
			 write(c);
		 }
	 }

	 void write(char c)  
	 {
		switch (c) 
		{
		case '<':
			*ostream << '&';
			*ostream << 'l';
			*ostream << 't';
			*ostream << ';';
			break;

		case '>':
			*ostream << '&';
			*ostream << 'g';
			*ostream << 't';
			*ostream << ';';
			break;

		case '&':
			*ostream << '&';
			*ostream << 'a';
			*ostream << 'm';
			*ostream << 'p';
			break;

		case '"':
			*ostream << '&';
			*ostream << 'q';
			*ostream << 'u';
			*ostream << 'o';
			*ostream << 't';
			*ostream << ';';
			break;

		case '\r':
		case '\n':
			*ostream << '&';
			*ostream << '#';
			*ostream << c;
			*ostream << ';';
			break;

		default:
			writeChar(c);
		}
	}

	 void writeChar(char c)  
	 {
		*ostream << ((c >> 8) & 0xFF);
		*ostream << ((c >> 0) & 0xFF);
	}

	 void write(char b)  {
		switch (b) {
		case '<':
			*ostream << '&';
			*ostream << 'l';
			*ostream << 't';
			*ostream << ';';
			break;

		case '>':
			*ostream << '&';
			*ostream << 'g';
			*ostream << 't';
			*ostream << ';';
			break;

		case '&':
			*ostream << '&';
			*ostream << 'a';
			*ostream << 'm';
			*ostream << 'p';
			break;

		case '"':
			*ostream << '&';
			*ostream << 'q';
			*ostream << 'u';
			*ostream << 'o';
			*ostream << 't';
			*ostream << ';';
			break;

		case '\r':
		case '\n':
			ostream.write('&');
			ostream.write('#');
			ostream.write(b);
			ostream.write(';');
			break;

		default:
			ostream.write(b);
		}
	}

	void setStream(std::stringstream * os) 
	{
		ostream = std::make_unique<std::stringstream>(os);
		*ostream << "<?xml version=\"1.0\"?>";
	}

private:
	std::unique_ptr<std::stringstream> ostream;
};