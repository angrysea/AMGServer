#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include "../server/IServer.h"
#include "BaseWriter.h"
#include "Message.h"

class PostWriter : public BaseWriter 
{

	PostWriter() 
	{
	}

	PostWriter(std::stringstream * ostream) :
		BaseWriter (ostream)
	{
	}

	void write(Message msg, const std::vector<std::string> & args)
	{
		// System.out.println();
		// System.out.println("*************Outgoing XML*********************");

		// Declared up here hopefully put into register.
		try {
			const Address & addr = msg.getAddress();
			writeString("<Envelope><Header><Message>");
			writeString("<To>");
			writeAddress(addr);
			writeString("</To>");

			const std::string & id = msg.getID();
			if (!id.empty())
			{
				writeString("<id>");
				writeString(id);
				writeString("</id>");
			}

			// Check to see if a certificate is available
			if (!IServer::getserver()->getCertificate().empty()) 
			{
				writeString("<Certificate>");
				writeString(IServer::getserver()->getCertificate());
				writeString("</Certificate>");
			}

			const std::string & key = IServer::getserver()->getKey();
			if (!key.empty())
			{
				writeString("<Key>");
				writeString(key);
				writeString("</Key>");
			}

			const std::string & timestamp = msg.getTimeStamp();
			if (!timestamp.empty())
			{
				writeString("<Timestamp>");
				writeString(timestamp);
				writeString("</Timestamp>");
			}

			const std::string & hops = msg.getHops();
			if (!hops.empty())
			{
				writeString("<Hops>");
				writeString(hops);
				writeString("</Hops>");
			}

			const Address & replyTo = msg.getReplyTo();
			writeString("<ReplyTo>");
			writeAddress(replyTo);
			writeString("</ReplyTo>");
			writeString("</Message>");
			writeString("</Header><Body>");

			for (auto a : args)
			{
				writeLine(a);
			}

			writeString("</Body></Envelope>");
		} 
		catch (std::exception t) 
		{
			//TODO: error handling
		}
	}

	void writeAddress(Address address)
	{
		writeString("<Address><Prefix>");
		writeString(address.getPrefix());
		writeString("</Prefix><Host>");
		writeString(address.getHost());
		writeString("</Host><Port>");
		writeString(address.getPort());
		writeString("</Port>");
		const std::string & postfix = address.getPostfix();
		if (!postfix.empty()) 
		{
			writeString("<Postfix>");
			writeString(postfix);
			writeString("</Postfix>");
		}
		const std::string & controller = address.getController();
		if (!controller.empty()) {
			writeString("<Controller>");
			writeString(controller);
			writeString("</Controller>");
		}
		const std::string & method = address.getMethod();
		if (!method.empty()) 
		{
			writeString("<Method>");
			writeString(method);
			writeString("</Method>");
		}
		const std::string & type = address.getType();
		if (!type.empty()) 
		{
			writeString("<Type>");
			writeString(type);
			writeString("</Type>");
		}
		const std::string email = address.getEmail();
		if (!email.empty()) 
		{
			writeString("<Email>");
			writeString(email);
			writeString("</Email>");
		}
		const Address & route = address.getRoute();
		writeString("<Route>");
		writeAddress(route);
		writeString("</Route>");
		writeString("</Address>");
	}
};