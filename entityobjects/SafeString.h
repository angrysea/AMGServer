#pragma once
#include "export.h"
#include <string>

namespace safteystring
{
	class ENTITYOBJECTS_API SafeString
	{
	public:
		SafeString() : sstring{ nullptr }
		{
		}
		SafeString(const SafeString & aCopy);
		SafeString(SafeString && source) :
			sstring{ source.sstring }
		{
			source.sstring = nullptr;
		}

		SafeString(const char * pSrc);
		SafeString(const std::string & source);
		SafeString(int len);
		SafeString(const char * pSrc, unsigned int len);
		~SafeString();
		SafeString & operator=(const std::string & source);
		SafeString & operator=(const SafeString & source);
		SafeString & operator=(SafeString && source);
		int SafeStringByteLen() const;
		int SafeStringLen() const;
		char * SafeStringToString() const;
		int SafeStringWriteImage(unsigned char * dest) const;
		int SafeStringReadImage(const unsigned char * src);
		unsigned char * getData()
		{
			unsigned char * src = new unsigned char[SafeStringByteLen()];
			SafeStringReadImage(src);
			return src;
		}

	private:
		int allocate(int len);
		void SafeString::free();

	private:
		char * sstring;
	};
};

#define SString char *

char * SafeString(const char * pSrc);
char * SafeString(int len);
char * SafeString(const char * pSrc, unsigned int len);
char * SafeString(const char * pSrc);
char * SafeString(const char * pSrc, unsigned int len);
void SafeStringFree(char * pSrc);
int SafeStringByteLen(char * sstring);
int SafeStringLen(char * sstring);
char * SafeStringToString(char * pSrc);
int SafeStringWriteImage(unsigned char * dest, SString theString);
int SafeStringReadImage(unsigned char * src, SString * theString);

