#pragma once

#include <string>

#include "Address.h"

/**
 * The Message class describes the actual message data transferred between
 * plugins.
 */
class Message 
{
public:
	Message();
	Message(const std::string & host, int nPort);
	Message(const Address & toAddress, const std::string & host, int nPort);
	Message(const std::string & toUri, const std::string & host, int nPort);
	Message(const std::string & uri);
	Message(const Message & msg);
	const std::string toString();
	std::size_t computeUID() const;
	const Address & getAddress() const;
	void setSync();
	void setAddress(const Address & newValue);
	void setReplyTo(const Address & newValue);
	const Address & getReplyTo() const;
	const std::string & getID() const;
	void setID(const std::string & newValue);
	const std::string & getTimeStamp() const;
	void setTimeStamp();
	void setTimeStamp(const std::string & newValue);
	const std::string & getHops() const;
	void setHops(const std::string & newValue);
	void setHops(int newValue);
	int getHopCount() const;
	const std::string & getCertificate() const;
	void setCertificate(const std::string & newValue);
	const std::string & getKey() const;
	void setKey(const std::string & newValue);
	void setKey(int newValue);
	int getKeyType() const;
	void setMethod(const std::string & method);
	void setPlugin(const std::string & controller);
	const Address hop() const;
	const Address & getRoute() const;
	bool getGuaranteed() const;
	void setGuaranteed(bool newValue);
	const std::string getURL() const;
	static const Message createReply(const Message & msg);

private:
	Address address;
	Address replyTo;
	std::string id;
	std::string timestamp;
	std::string hops;
	std::string key;
	std::string certificate;
	bool guaranteed = false;
};
