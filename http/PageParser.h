#pragma once

#include "stdafx.h"
#include "BaseParser.h"
#include "Page.h"

class PageParser : public BaseParser 
{
public:
	PageParser(Socket & s, const std::string & t) 
		:BaseParser(s),
		type(t) 
	{
    }

	PageParser(const std::string & s, const std::string & t) 
		:BaseParser(s),
		type(t) 
	{
    }

    Page * parse() 
	{
		Page * page = new Page;
        std::string length;

        page->setVersion(getValue('\n',false));
        parseHeaders(page);

		reset();
		do {
			ch=reader->get();
			append((char)ch);
		} while(ch != BufferedReader::eof); 
		page->setPage(getBuffer());

		return page;
    }

protected:
	void parseHeaders(Page * page) 
	{
        while(true) 
		{
    	    if(ch=='\r') 
			{
	            if((ch = reader->get())=='\n')
        		    break;
	        }
            else if(ch =='\n') 
			{
        		break;
	        }

    	    std::string name=getHeaderName();
    	    skipWhiteSpace();
    	    parseHeader();
            //name=name.toLowerCase();
            page->putProperty(name,getBuffer());
        }

		while(true) 
		{
			ch = reader->get();
    	    if(ch=='\r') 
			{
	            if((ch = reader->get())=='\n')
        		    break;
	        }
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
