#pragma once

#include <string>

#include "Body.h"
#include "Header.h"

/*******************************************************************************
 * Envelope for API execution. Envelope contains the Header and Body information
 * of the transaction request.
 ******************************************************************************/

class Envelope 
{
public:
	Envelope() 
	{
	}

	const Header & getHeader() const
	{
		return header;
	}

	void setHeader(Header newValue) 
	{
		header = newValue;
	}

	const Body & getBody() const
	{
		return body;
	}

	void setBody(Body newValue)
	{
		body = newValue;
	}

	const std::string & getController() const
	{
		return  header.getMessage().getAddress().getController();
	}
	
	int getHopCount() const
	{
		return  header.getMessage().getHopCount();
	}

	int getUID() 
	{
		return  header.getMessage().computeUID();
	}

	const std::string & getContent(int idx) const
	{
		return body.get(idx);
	}

	bool isMethod(const std::string & method) const
	{
		return header.getMessage().getAddress().getMethod() == method;
	}

	bool isSync() const
	{
		return header.getMessage().getAddress().isSync();
	}

	const Address & getReplyTo() const
	{
		return header.getMessage().getReplyTo();
	}
private:
	Header header;
	Body body;
};
