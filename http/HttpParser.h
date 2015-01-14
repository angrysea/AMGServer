#pragma once

#include "stdafx.h"
#include "BaseParser.h"
#include "request.h"

class HttpParser : public BaseParser 
{
public:
	HttpParser(Socket & s, const std::string & t) 
		:BaseParser(s),
		type(t) 
	{
    }

	HttpParser(const std::string & s, const std::string & t) 
		:BaseParser(s),
		type(t) 
	{
    }

    Request * parse() 
	{
		Request * request = new Request;
        std::string length;

        try 
		{
            request->setVersionMethod(getValue('\n',false));
            parseHeaders(request);

            std::string method=request->getMethod();
            if(method.length()>0) 
			{
				if(method==Request::POST) 
				{
					length=request->getProperty("Content-Length");
					if(length.length()>0) 
					{
						int nLength=atoi(length.c_str());
                        int nRead=0;
                        if(nLength>0) 
						{
                            char * data = new char[nLength+1];
							data[nLength]=0x00;
                            int ch=0;

                            for(int i=0;i<nLength&&ch!=BufferedReader::eof;i++) 
							{
								ch=reader->get();                   
								data[i]=(char)ch;
                            }
							request->setRequest(data, nLength);
							while(reader->get()!=BufferedReader::eof);
                        }
                    }
				}
            }
        }
        catch(...) 
		{
        }

	    return request;
    }

protected:
	void parseHeaders(Request * request) 
	{
        while(true) 
		{
    	    if(ch=='\r') 
			{
	            if((ch = reader->get())=='\n')
        		    return;
	        }
            else if(ch =='\n') 
			{
        		return;
	        }

    	    std::string name=getHeaderName();
    	    skipWhiteSpace();
    	    parseHeader();
            //name=name.toLowerCase();
            request->putProperty(name,getBuffer());
        }
    }

    std::string getHeaderName() 
	{
	    nSize=0;
	    while((ch >= 32)&&(ch!=':')) 
		{
	        append((char)ch);
	        ch=reader->get();
	    }

        std::string ret;
	    if(ch==':') 
		{
	        ch=reader->get();
	        if(nSize>0) 
			{
                ret=getBuffer();
            }
        }

	    return ret;
    }

    void parseHeader() 
	{
        bool bContinue=true;
	    nSize=0;

        skipWhiteSpace();
 
    	while(bContinue) 
		{
            switch(ch) 
			{
            case BufferedReader::eof:
                bContinue=false;
                break;

            case '\r':
                if((ch=reader->get())!='\n') 
				{
                    append('\r');
                    continue;
                }

            //fall-thru
            case '\n':
                switch (ch = reader->get()) 
				{
                case ' ':
                case '\t':
                    skipWhiteSpace();
                    append(ch);
                    break;
                default:
                    bContinue=false;
                    break;
                }
                break;

            default:
                append((char)ch);
                ch=reader->get();
                break;
            }
        }
    }

protected:
	void getVersion(std::string version)
	{
    }

private:
	std::string type;
};
