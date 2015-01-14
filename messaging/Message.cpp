#include <functional>
#include <chrono>

#include "Message.h"

/**
* Constructs an empty Message object
*/
Message::Message()
{
	timestamp =
		std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count());
}

/**
* Constructs an empty Message object
*/
Message::Message(const std::string & host, int nPort) :
replyTo{ host, nPort, false }
{
	timestamp =
		std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count());
}

/**
* Constructs a Message object with the specified destination address
*
* @param toAddress
*            destination address of this message
*/
Message::Message(const Address & toAddress, const std::string & host, int nPort) : address{ toAddress },
replyTo{ host, nPort, false }
{
	timestamp =
		std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count());

}

/**
* Constucts a Message object initialized with the specifed message.
*
* @param msg
*            Message to use for initialization
*/
Message::Message(const Message & msg) :
	address{ msg.address },
	replyTo{ msg.replyTo },
	id{ msg.id },
	timestamp{ msg.timestamp },
	hops{ msg.hops },
	guaranteed{ msg.guaranteed }
{
}

const std::string Message::toString()
{
	std::string sb;
	sb.append("Address: ");
	sb.append(address.tostring());
	sb.append("\nreplyto: ");
	sb.append(replyTo.tostring());
	sb.append("\ntimestamp: ");
	sb.append(timestamp);
	sb.append("\nhops: ");
	sb.append(hops);
	sb.append("\nkey: ");
	sb.append(key);
	sb.append("\ncertificate: ");
	sb.append(certificate);

	return sb;
}

std::size_t Message::computeUID() const
{
	std::string sb;
	sb.append(replyTo.getHost());
	sb.append(replyTo.getPort());
	sb.append(address.getController());
	sb.append(address.getMethod());
	sb.append(timestamp);
	std::hash<std::string> hash_fn;

	return hash_fn(sb);
}

/**
* Retrives the destinaion address of this message
*
* @return destination address
*/
const Address & Message::getAddress() const
{
	return address;
}

void Message::setSync()
{
	address.setSync();
}

/**
* Sets the destination address of this message
*
* @param address
*            destination address
*/
void Message::setAddress(const Address & newValue)
{
	address = newValue;
}

/**
* Sets the reply to address of this message
*
* @param replyTo
*            reply to address
*/
void Message::setReplyTo(const Address & newValue)
{
	replyTo = newValue;
}

/**
* Retrieves the reply to address for this message
*
* @return reply to address
*/
const Address & Message::getReplyTo() const
{
	return replyTo;
}

/**
* Retrieves the ID of this message
*
* @return message ID
*/
const std::string & Message::getID() const
{
	return id;
}

/**
* Sets the ID for this message
*
* @param id
*            ID to set
*/
void Message::setID(const std::string & newValue)
{
	id = newValue;
}

/**
* Retrieves the timestamp of this message
*
* @return message timestamp
*/
const std::string & Message::getTimeStamp() const
{
	return timestamp;
}

/**
* Sets the timestamp of this message.
*/
void Message::setTimeStamp()
{
	timestamp =
		std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count());
}

/**
* Sets the timestamp of this message using the specified timestamp
*
* @param timestamp
*            timestamp to set
*/
void Message::setTimeStamp(const std::string & newValue)
{
	timestamp = newValue;
}

/**
* Retrieves the hop count for this message as a string
*
* @return hop count in std::string form
*/
const std::string & Message::getHops() const
{
	return hops;
}

/**
* Sets the hop count for this message
*
* @param hops
*            hop count in std::string form
*/
void Message::setHops(const std::string & newValue)
{
	hops = newValue;
}

/**
* Sets the hop count for this message
*
* @param hops
*            hop count in int form
*/
void Message::setHops(int newValue)
{
	hops = std::to_string(newValue);
}

/**
* Retrieves the hop count for this message
*
* @return hop count
*/
int Message::getHopCount() const
{
	return std::stoi(hops);
}

/**
* Retrieves the certificate of this object
*
* @return message certificate in std::string form
*/
const std::string & Message::getCertificate() const
{
	return certificate;
}

/**
* Sets the certificate for this message
*
* @param certificate
*            message certificate in std::string form
*/
void Message::setCertificate(const std::string & newValue)
{
	certificate = newValue;
}

/**
* Retrieves the key of this object
*
* @return message key in std::string form
*/
const std::string & Message::getKey() const
{
	return key;
}

/**
* Sets the key for this message
*
* @param key
*            message key in std::string form
*/
void Message::setKey(const std::string & newValue)
{
	key = newValue;
}

/**
* Sets the key for this message
*
* @param key
*            message key
*/
void Message::setKey(int newValue)
{
	key = std::to_string(newValue);
}

/**
* Retrieves the key of this object
*
* @return message key
*/
int Message::getKeyType() const
{
	return std::stoi(key);
}

/**
* Sets the method name for this messages
*
* @param method
*            method name
*/
void Message::setMethod(const std::string & method)
{
	address.setMethod(method);
}

/**
* Sets the plugin name for this message
*
* @param plugin
*            plugin name
*/
void Message::setPlugin(const std::string & controller)
{
	address.setController(controller);
}

const Address Message::hop() const
{
	return address.hop();
}

const Address & Message::getRoute() const
{
	return address.getRoute();
}

bool Message::getGuaranteed() const
{
	return guaranteed;
}

void Message::setGuaranteed(bool newValue)
{
	guaranteed = newValue;
}

const std::string Message::getURL() const
{
	return address.getURL();
}

/**
* Constructs a new reply message for the specified message
*
* @param msg
*            message to reply to
* @return new reply message
*/
const Message Message::createReply(const Message & msg)
{
	Message message;
	message.address = msg.replyTo;
	message.replyTo = msg.address;
	message.id = msg.id;
	message.timestamp = msg.timestamp;
	message.hops = msg.hops;
	return message;
}

