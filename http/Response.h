#pragma once

#include "stdafx.h"
#include "HTTP.h"

class Response
{
public: 
	
	Response() 
		:host("localhost"),
		 nStatus(200),
		 writer(0)
	{
    }

	Response(Socket & socket, std::string newHost, std::string contentType)
		:host(newHost),
		 nStatus(200),
         contentType(contentType)
	{
		writer = new BufferedSocketWriter(socket);
	}

	void writeHeader(long contentLength)  
	{

		writer->write(HTTP::byteArrayVersion);
        writer->write(nStatus);

        std::string strCode = "OK";
        if (nStatus<200)
		    strCode = HTTP::msg100[nStatus-100];
        else if (nStatus<300)
		    strCode = HTTP::msg200[nStatus-200];
		else if (nStatus<400)
		    strCode = HTTP::msg300[nStatus-300];
        else if (nStatus<500)
		    strCode = HTTP::msg400[nStatus-400];
        else if (nStatus<600)
            strCode = HTTP::msg500[nStatus-500];

        writer->write(strCode.c_str());
        writer->write(HTTP::crlf.c_str());

        writer->write(HTTP::transceiver.c_str());
		writer->write(HTTP::crlf.c_str());

		/*
        writer->write(HTTP::date.c_str());
        //writer->write(getDate());
		writer->write(HTTP::crlf.c_str());
		*/
        writer->write(HTTP::nocache.c_str());
        writer->write(HTTP::crlf.c_str());

        if (nStatus == HTTP::UNAUTHORIZED)
        {
            writer->write("WWW-Authenticate: Basic realm=\"Transceiver\"");
            writer->write(HTTP::crlf.c_str());
            writer->write(HTTP::crlf.c_str());
            writer->flush();
            return;
        }

        if (contentType.size()==0)
        {
            writer->write(HTTP::contentTypeXML.c_str());
        }
        else
        {
            writer->write(contentType.c_str());
        }
		writer->write(HTTP::crlf.c_str());
		writer->write(HTTP::acceptRange.c_str());
		writer->write(HTTP::crlf.c_str());
/*
		writer->write(HTTP::lastModified.c_str());
		//writer->write(getDate());
		writer->write(HTTP::crlf.c_str());
*/
        if (contentLength != 0)
        {
            writer->write(HTTP::contentLength.c_str());
		    writer->write(contentLength);
	        writer->write(HTTP::crlf.c_str());
		}
        writer->write(HTTP::crlf.c_str());

		if (nStatus >= 400) {
            writer->write("<html><head><title>");
            writer->write(nStatus);
            writer->write(" ");
            writer->write(strCode.c_str());
            writer->write("</title></head><h1>");
            writer->write(strCode.c_str());
            writer->write("</h1></html>");
		}
        writer->flush();
    }

    void respond(char * response)
	{
        writeHeader((int)strlen(response));

        writer->write(response);
        writer->write(HTTP::crlf.c_str());
        writer->flush();
	}

    void setHost(std::string newValue)
	{
		host=newValue;
	}

	void setStatus(int newValue)
	{
		nStatus=newValue;
	}

private:
	/*
	std::string getDate()
    {
        SimpleDateFormat format = new SimpleDateFormat("EEE, dd MMM, yyyy hh:mm:ss zzz");
        return format.format(new Date());
    }
	*/
public:
    BufferedSocketWriter * getSocketWriter() { return writer; }

private:
	std::string host;
	int nStatus;
    BufferedSocketWriter * writer;
    std::string contentType;
};
