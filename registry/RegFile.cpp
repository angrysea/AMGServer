
#include "RegFile.h"


bool RegFile::openRead(std::string & fileName)
{
	errno_t err;
	beof = false;
	err = _set_fmode(_O_BINARY);

	if ((_sopen_s(&fh, fileName.c_str(), _O_RDONLY, _SH_DENYNO, _S_IREAD))!=0) {
		std::cout << "Cannot open source file: " << std::endl;
		return false;
    }

	char buf[2];
	bytesRead = _read(fh, buf, 2);

	// Check reg signature
	if(buf[0]!=-1 && buf[1]!=-2)
	{
		return false;
	}

	if(loadBuffer()==0)
	{
		return false;
	}

	return true;
}

bool RegFile::openWrite(std::string & fileName)
{
	errno_t err;
	beof = false;
	err = _set_fmode(_O_BINARY);

	if ((_sopen_s(&fh , fileName.c_str(), _O_WRONLY | _O_CREAT | _O_TRUNC, _SH_DENYNO, _S_IWRITE ))!=0) {
		std::cout << "Cannot open target file: " << std::endl;
		return false;
    }
	
	pos=0;
	// Write reg signature
	char buf[2];
	buf[pos++]=-1;
	buf[pos++]=-2;
	bytesRead = _write(fh, buf, 2);

	return true;
}

bool RegFile::getLine(std::string & line)
{
	if(pos==bytesRead)
	{
		loadBuffer();
	}

	if(bytesRead==0)
	{
		return false;
	}

	char linebuf[MAX_BUFFER];
	memset(linebuf, 0x00, MAX_BUFFER*2);

	bool bRet = false;
	bool last13;
	char c;
	for(int i=0; getNext(c) && i<MAX_BUFFER; i++) 
	{
		if(c==13)
		{
			last13=true;
		}
		else if(c==10)
		{
			if(last13)
			{
				line = linebuf;
				bRet = true;
				break;
			}
		}
		else
		{
			last13=false;
			linebuf[i]=c;
		}
	}

	return bRet;
}

int RegFile::writeLine(std::string & line)
{
	int bytesWritten = 0;
	if(line.length()>0)
	{
		if ((bytesWritten=_write(fh, line.c_str(), line.length()*2))<=0) { 
			std::cout << "Error writing to target file: " << std::endl;
			return -1;
		}
		bytesWritten/=2;

		char buf[2];
		buf[0] = 13;
		buf[1] = 10;
		int check =0;
		if ((check=_write(fh, buf, 4))<=0) { 
			std::cout << "Error writing to target file: " << std::endl;
			return -1;
		}
		bytesWritten+=2;
	}

	return bytesWritten;
}

int RegFile::blankLine()
{
	int bytesWritten = 0;
	char buf[2];
	buf[0] = 13;
	buf[1] = 10;
	int check =0;
	if ((check=_write(fh, buf, 4))<=0) { 
		std::cout << "Error writing to target file: " << std::endl;
		return -1;
	}
	bytesWritten+=2;

	return bytesWritten;
}


bool RegFile::getNext(char & c)
{
	if(pos==bytesRead)
	{
		loadBuffer();
	}
	if(beof)
	{
		return false;
	}
	c=wbuf[pos++];
	return true;
}

int RegFile::loadBuffer()
{
	pos = 0;
	char buf[MAX_BUFFER*2];
	bytesRead = _read(fh, buf, MAX_BUFFER*2);

	if(bytesRead>0)
	{
		memset(wbuf, 0x00, MAX_BUFFER*2);
		memcpy(wbuf, buf, MAX_BUFFER*2);
		bytesRead/=2;
	}
	else
	{
		beof = true;
	}

	return bytesRead;
}

int RegFile::close()
{
    if (_close(fh) == EOF) {
		std::cout << "Error when closing source file: " << std::endl;
		return -1;
    }
	return 0;
}

