#include <functional>
#include <chrono>
#include <sstream>

#include "PeerEntry.h"

PeerEntry::PeerEntry()
{
	timestamp = 0;
}

PeerEntry::PeerEntry(const std::string & url) :
address{ url }
{
	timestamp =
		std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count();
}

PeerEntry::PeerEntry(const Address & addr) :
address{ addr }
{
	timestamp =
		std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count();
}

PeerEntry::PeerEntry(const std::string & host, int port, bool bSecure)
{
	std::string uri;
	if (bSecure == false)
		uri.append("http");
	else
		uri.append("https");

	uri.append("://");
	uri.append(host);
	uri.append(":");
	uri.append(std::to_string(port));
	address.setURI(uri);
	timestamp =
		std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count();
}

PeerEntry::PeerEntry(const PeerEntry & pe) :
address{ pe.address },
name{ pe.name },
type{ pe.type },
timestamp{ pe.timestamp }
{
}

PeerEntry::PeerEntry(const PeerEntry && pe) :
address{ std::move(pe.address) },
name{ std::move(pe.name) },
type{ std::move(pe.type) },
timestamp{ std::move(pe.timestamp) }
{
}

PeerEntry & PeerEntry::operator=(const PeerEntry & pe)
{
	address = pe.address;
	name = pe.name;
	type = pe.type;
	timestamp = pe.timestamp;

	return *this;
}

PeerEntry & PeerEntry::operator=(const PeerEntry && pe)
{
	address = std::move(pe.address);
	name = std::move(pe.name);
	type = std::move(pe.type);
	timestamp = std::move(pe.timestamp);

	return *this;
}

void PeerEntry::setAddress(const std::string & uri)
{
	address.setURI(uri);
}

const Address & PeerEntry::getAddress() const
{
	return address;
}

const Address * const PeerEntry::getAddressPtr() const
{
	return &address;
}

int PeerEntry::getKey() const
{
	return address.hashCode();
}

void PeerEntry::setURL(const std::string & url)
{
	address.setURL(url);
}

const std::string PeerEntry::getURL() const
{
	return address.getURL();
}

const std::string PeerEntry::getNameURL() const
{
	return address.getNameURL();
}

void PeerEntry::setName(const std::string & newValue)
{
	name = newValue;
}

const std::string & PeerEntry::getName() const
{
	return name;
}

void PeerEntry::setEmail(const std::string & newValue)
{
	address.setEmail(newValue);
}

const std::string PeerEntry::getEmail() const
{
	return address.getEmail();
}

void PeerEntry::setType(const std::string & newValue)
{
	type = newValue;
}

const std::string & PeerEntry::getType()
{
	return type;
}

void PeerEntry::setTime(const std::string & newValue)
{
	if (!newValue.empty())
		setTime(std::atoi(newValue.c_str()));
}

void PeerEntry::setTime(long newValue)
{
	timestamp = newValue;
}

long PeerEntry::getTime() const
{
	return timestamp;
}

bool PeerEntry::isAlive() const
{
	return bAlive;
}

void PeerEntry::setAlive(bool newValue)
{
	bAlive = newValue;
}

const std::string PeerEntry::getTimeAsString() const
{
	return std::to_string(timestamp);
}

bool PeerEntry::operator==(const PeerEntry & pe) const
{
	return (address == pe.address &&
		name == pe.name &&
		type == pe.type);
}
