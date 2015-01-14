#pragma once

#include "../server/IServer.h"
#include "Message.h"

class Header 
{
public:
	Header() 
	{
	}

	Header(IServer * transceiver) : 
		message{ transceiver, false }
	{		
	}

	Header(IServer * transceiver, bool bSecure) :
		message{ transceiver, bSecure }
	{
	}

	const Message & getMessage() const
	{
		return message;
	}

	void setMessage(Message newValue) 
	{
		message = newValue;
	}

private:
	Message message;
};
