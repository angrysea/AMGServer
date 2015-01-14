#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <exception>

#include "../server/IServer.h"
#include "BaseWriter.h"
#include "Message.h"

class ResponseWriter : public BaseWriter 
{
public:
	ResponseWriter(std::stringstream * ostream) :
		BaseWriter{ ostream }
	{
	}

	ResponseWriter(std::stringstream * ostream, bool bHeader) : 
		BaseWriter{ ostream, bHeader }
	{
	}

	void writeResponse(const std::string & ret)
	{
		writeString("<Envelope><Body>");
		writeString(ret);
		writeString("</Body></Envelope>");
	}
	
	void writeResponse(const std::string & ret) 
	{
		// Declared up here hopefully put into register.
		try 
		{
			writeString(ret);
		} 
		catch (exception t) 
		{
		}
	}

	void writeResponseBytes(const std::string & ret)
	{
		// Declared up here hopefully put into register.
		try 
		{
			for (auto c : ret)
			{
				write(c);
			}
		} 
		catch (std::exception t) 
		{
		}
	}
};