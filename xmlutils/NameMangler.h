#pragma once

#include <iostream>
#include <string>
#include <cctype> 
#include <exception> 

#define REPLACECHAR '_'
#define MANGLERCHAR '$'
#define DASHCODE 'D'
#define PERIODCODE 'P'
#define COLONCODE 'C'

class NameMangler 
{
public:
	NameMangler() : 
		DEBUG{ false }
	{
	}

	const std::string encode(const std::string & name)
	{
		const std::string disallowedChars{ "-.:" };
		std::string buff;

		if (name.empty())
		{
			return name;
		}

		int pos = std::string::npos;
		bool bMangled = false;
		for (auto & c : name)
		{
			if (disallowedChars.find_first_of(c) != std::string::npos)
			{
				buff+=REPLACECHAR;
				appendCharCode(buff, c);
				bMangled = true;
			}
			else
			{
				buff += c;
			}
		}

		if (bMangled==true)
		{
			buff+=MANGLERCHAR;
		}

		return buff;
	}

	std::string decode(std::string & name)
	{
		std::string buff;
		if (name.empty())
		{
			return name;
		}
		buff.resize(name.length() - 2);
		if (name.find_last_of(MANGLERCHAR) != std::string::npos)
		{
			bool bReplaceCharFound = false;
			for (auto & c : name)
			{
				if (c == MANGLERCHAR)
				{
					continue;
				}
				else if (bReplaceCharFound == true)
				{
					bReplaceCharFound = false;
					buff += getDecodedChar(c);
					continue;
				}
				else if (c == REPLACECHAR)
				{
					bReplaceCharFound = true;
					continue;
				}
				buff += c;
			}
		}
		return buff;
	}

private:
	void appendCharCode(std::string & buff, char ch) 
	{
		switch (ch) 
		{
		case '-':
			buff+=DASHCODE;
			break;
		case '.':
			buff+=PERIODCODE;
			break;
		case ':':
			buff+=COLONCODE;
			break;
		default:
			break;
		}
	}

	char getDecodedChar(char ch) 
	{
		char chOut = 0;
		switch (ch) 
		{
		case DASHCODE:
			chOut = '-';
			break;
		case PERIODCODE:
			chOut = '.';
			break;
		case COLONCODE:
			chOut = ':';
			break;
		default:
			break;
		}

		return chOut;
	}

private:
	bool DEBUG;
};
