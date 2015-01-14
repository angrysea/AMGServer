#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SafeString.h"

namespace safteystring
{
	SafeString::SafeString(const SafeString & aCopy)
	{
		memcpy(sstring, aCopy.sstring, allocate(aCopy.SafeStringLen()));
	}

	SafeString::SafeString(const char * pSrc) :
		SafeString{ pSrc, strlen(pSrc) }
	{
	}

	SafeString::SafeString(const std::string & source) :
		SafeString{ source.c_str(), source.length() }
	{
	}

	SafeString::SafeString(int len) : sstring{ nullptr }
	{
		allocate(len);
	}

	SafeString::SafeString(const char * pSrc, unsigned int len) : sstring{ nullptr }
	{
		if (pSrc != nullptr)
		{
			memcpy(sstring, pSrc, allocate(len));
		}
	}

	SafeString::~SafeString()
	{
		free();
	}

	SafeString & SafeString::operator=(const std::string & source)
	{
		free();
		int size = allocate(source.length());
		memcpy(sstring, source.c_str(), size);
		return *this;
	}

	SafeString & SafeString::operator=(SafeString && source)
	{
		free();
		sstring = source.sstring;
		source.sstring = nullptr;
		return *this;
	}

	int SafeString::allocate(int len)
	{
		unsigned int size = 0;
		sstring = nullptr;
		if (len > 0)
		{
			size = ((len + 1) * sizeof(char)+sizeof(int));
			char * buffer = new char[size];
			memset(buffer, 0x00, size);
			memcpy(buffer, &len, sizeof(int));
			sstring = (buffer + sizeof(int));
		}
		return size;
	}

	void SafeString::free()
	{
		if (sstring != nullptr)
		{
			char * p = sstring - sizeof(int);
			delete p;
			sstring = nullptr;
		}
	}

	int SafeString::SafeStringByteLen() const
	{
		int len = 0;
		if (sstring != 0)
		{
			len = ((SafeStringLen() + 1)*sizeof(char)) + sizeof(int);
		}
		else
		{
			len = sizeof(int);
		}
		return len;
	}

	int SafeString::SafeStringLen() const
	{
		int len = 0;
		if (sstring != 0)
		{
			char * p = sstring - sizeof(int);
			memcpy(&len, p, sizeof(int));
		}
		return len;
	}

	char * SafeString::SafeStringToString() const
	{
		size_t size = strlen(sstring);
		char * buf = new char[size + 1];
		strcpy(buf, sstring);
		return buf;
	}

	/*
	 * Writes the binary image of theString to the buffer dest.
	 * Returns the number of bytes written to the buffer.
	 */
	int SafeString::SafeStringWriteImage(unsigned char * dest) const
	{
		int len = 0;

		if (sstring != 0)
		{
			len = SafeStringByteLen();
			memcpy(dest, (sstring - sizeof(int)), len);
		}
		else
		{
			len = sizeof(int);
			memset(dest, 0x00, len);
		}
		return len;
	}

	/*
	 * Writes the binary image of theString to the buffer dest.
	 * Returns the number of bytes written to the buffer.
	 */
	int SafeString::SafeStringReadImage(const unsigned char * src)
	{
		int dataUsed = sizeof(int);
		int len = *(int *)src;
		src += sizeof(int);

		if (len > 0)
		{
			int size = allocate(len);
			memcpy(sstring, src, size);
			dataUsed += ((len + 1)*sizeof(char));
		}
		else
		{
			sstring = 0;
		}
		return dataUsed;
	}
}

char * SafeString(const char * pSrc)
{
	char * sstring = 0;
	if (pSrc != 0)
	{
		sstring = SafeString(pSrc, strlen(pSrc));
	}
	return sstring;
}

char * SafeString(int len)
{
	char * sstring = 0;
	if (len>0)
	{
		unsigned int size = ((len + 1) * sizeof(char)+sizeof(int));
		char * buffer = new char[size];
		memset(buffer, 0x00, size);
		memcpy(buffer, &len, sizeof(int));
		sstring = (buffer + sizeof(int));
	}
	return sstring;
}

char * SafeString(const char * pSrc, unsigned int len)
{
	char * sstring = 0;
	if (pSrc == 0)
	{
		sstring = SafeString(len);
	}
	else if (len>0)
	{
		unsigned int size = ((len + 1) * sizeof(char));
		char * buffer = new char[size + sizeof(int)];
		memset(buffer, 0x00, size + sizeof(int));
		memcpy(buffer, &len, sizeof(int));
		sstring = (char *)(buffer + sizeof(int));
		memcpy(sstring, pSrc, size);
	}
	return sstring;
}

int SafeStringByteLen(char * sstring)
{
	int len = 0;
	if (sstring != 0)
	{
		len = ((SafeStringLen(sstring) + 1)*sizeof(char)) + sizeof(int);
	}
	else
	{
		len = sizeof(int);
	}
	return len;
}

int SafeStringLen(char * sstring)
{
	int len = 0;
	if (sstring != 0)
	{
		char * p = sstring - sizeof(int);
		memcpy(&len, p, sizeof(int));
	}
	return len;
}


void SafeStringFree(char * pSrc)
{
	if (pSrc != 0)
	{
		char * p = pSrc - sizeof(int);
		delete p;
	}
}

char * SafeStringToString(char * pSrc)
{
	size_t size = strlen(pSrc);
	char * buf = new char[size + 1];
	strcpy(buf, pSrc);
	return buf;
}

/*
* Writes the binary image of theString to the buffer dest.
* Returns the number of bytes written to the buffer.
*/
int SafeStringWriteImage(unsigned char * dest, SString sstring)
{
	int len = 0;

	if (sstring != 0)
	{
		len = SafeStringByteLen(sstring);
		memcpy(dest, (sstring - sizeof(int)), len);
	}
	else
	{
		len = sizeof(int);
		memset(dest, 0x00, len);
	}
	return len;
}

/*
* Writes the binary image of theString to the buffer dest.
* Returns the number of bytes written to the buffer.
*/
int SafeStringReadImage(unsigned char * src, SString * theString)
{
	int dataUsed = sizeof(int);
	int len = *(int *)src;
	src += sizeof(int);

	if (len>0)
	{
		*theString = SafeString((const char *)src, len);
		dataUsed += ((len + 1)*sizeof(char));
	}
	else
	{
		theString = 0;
	}
	return dataUsed;
}

