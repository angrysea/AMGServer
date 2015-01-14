#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <functional>
#include <chrono>

#include "message.h"

class ControllerAgent;
class NetworkAgent;

class Messenger 
{
public:
	Messenger();

	bool doTestConnection(const Message & msg);
	void openConnection(const std::string & address, bool bSecure);
	const std::string & completeConnection();
	static void postError(const Message & msg, const std::string & error);
	static bool testConnection(Message & msg);

	template <typename ...T>
	static void postMessage(Message & message, T &&... arguments)
	{
		std::thread t([message, arguments]()
		{
			Messenger messenger;
			messenger.doPostMessage(message, std::forward<T>(arguments));
		});
	}

	template <typename ...T>
	static const std::string & sendMessage(Message & msg, T &&... arguments)
	{
		msg.setSync();
		std::string ret = Messenger().doPostMessage(msg, td::forward<T>(arguments));
		//XMLReader parser = new XMLReader();
		//parser.setContentHandler(new MessageParser());
		Envelope env;
		//Envelope env = (Envelope)parser.parse(new InputSource(
		//	new ByteArrayInputStream(ret.getBytes())));
		return env.getContent(0);
	}

	template <typename ...T>
	static void localPostMessage(Message & message, T &&... arguments)
	{
		std::thread t([message, arguments]()
		{
			IServer * server = IServer::getTransceiver();
			std::string name = msg.getAddress().getcontroller();
			ControllerAgent * controller = (ControllerAgent*)server.getAvailablecontroller(name);

			if (controller != nullptr) 
			{
				Header header;
				header.setMessage(msg);
				Envelope env;
				env.setHeader(header);
				Body body;
				body.setcontentArray(args);
				env.setBody(body);
				controller->pushMessage(env);
				server->run(controller);
			}
		}
	}

	template <typename ...T>
	static void localPostMessage(const ControllerAgent * controller, const std::string & method,
		T &&... arguments);

	template <typename ...T>
	static void broadcastMessage(Message & message, T &&... arguments);

	template <typename ...T>
	static void broadcastMessage(Message & message, int hops, T &&... arguments);

private:
	template <typename ...T>
	const std::string & doPostMessage(Message & msg, T &&... args)
	{
		String ret = null;
		try {
			if (useProxy == false) {
				Address receiver = msg.hop();
				String url = receiver.getURL();
				String pst = receiver.getPostfix();
				if (pst != null) {
					url += "/" + pst;
				}

				/*
				std::cout << "=============== Outgoing message ===============\n";
				std::cout << "Sending to: " + url;
				std::cout << msg.toString();
				std::cout << "Args: ";
				for(auto arg : args) {
				std::cout << arg);
				}
				std::cout << "================================================\n");
				*/

				openConnection(url, false);
				new PostWriter(os).write(msg, args);
				ret = completeConnection();
			}
			else {
				proxyArgs[0] = msg;
				proxyArgs[1] = args;
				String url = msg.getAddress().getURL();
				openConnection(url, false);
				new PostWriter(os).write(proxyMessage, proxyArgs);
				ret = completeConnection();
			}
		}
		finally {
			if (is != null)
				is.close();
			if (os != null)
				os.close();
		}
		return ret;
	}



private:
	Message proxyMessage;

	static bool autoconnect;
	static  int MAXHOPS;
	static NetworkAgent * networkAgent;
	static bool useProxy;
};
