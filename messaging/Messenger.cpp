#include <thread>

#include "Messenger.h"
#include "Envelope.h"

#include "../server/IServer.h"

NetworkAgent * Messenger::networkAgent = nullptr; // IServer::getserver()->getService("networkagent");
bool Messenger::useProxy = IServer::getserver()->useProxy();
bool Messenger::autoconnect = false; //IServer::getserver().getProperty(ServerConfig.AUTOCONNECT)=="true" ? true : false;
int Messenger::MAXHOPS = 4;

Messenger::Messenger() :
proxyMessage{ IServer::getserver()->getHost(), IServer::getserver()->getHost(), false }
{
	if (useProxy == true) 
	{
		Address proxyAddress;
		proxyAddress.setURI(IServer::getserver()->getHost());
		proxyAddress.setController("ProxyServer");
		proxyAddress.setMethod("sendRequest");
		proxyMessage.setAddress(proxyAddress);
	}
}


bool Messenger::doTestConnection(const Message & msg)
{
	bool connected = false;
		Address receiver = msg.hop();
		std::string address = receiver.getURL();
		std::string pst = receiver.getPostfix();
		if (pst != nullprt) 
		{
			address += "/" + pst;
		}
		HttpTimeoutHandler xHTH = new HttpTimeoutHandler(10);
		URL url = new URL((URL)null, address, xHTH);
		connection = (HttpURLConnection)url.openConnection();
		connection.connect();
		connection.disconnect();
		connected = true;
	return connected;
}

void Messenger::openConnection(std::string address, bool bSecure)
{
		URL url = new URL(address);
		connection = (HttpURLConnection)url.openConnection();
		connection.setRequestProperty("Content-Type", " " + "text/xml");
		connection.setRequestProperty("Connection", "Keep-Alive");
		// connection.setRequestProperty("Connection", " close");
		connection.setDoOutput(true);
		connection.setAllowUserInteraction(true);
		os = connection.getOutputStream();
}

std::string Messenger::completeConnection() 
{
		connection.connect();
		os.flush();
		os.close();

		is = connection.getInputStream();
		StringBuffer stringbuffer = new StringBuffer();

		// read the response
		int b = -1;
		while (true) {
			b = is.read();
			if (b == -1)
				break;
			stringbuffer.append((char)b);
		}

		is.close();
		os.close();
		is = null;
		os = null;

		return stringbuffer.toString();
}
