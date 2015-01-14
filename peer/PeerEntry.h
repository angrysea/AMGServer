#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../messaging/Address.h"

class PeerEntry
{
public:
	PeerEntry();
	PeerEntry(const std::string & url);
	PeerEntry(const Address & addr);
	PeerEntry(const std::string & host, int port, bool bSecure);
	PeerEntry(const PeerEntry & pe);
	PeerEntry(const PeerEntry && pe);
	PeerEntry & operator=(const PeerEntry & pe);
	PeerEntry & operator=(const PeerEntry && pe);
	void setAddress(const std::string & uri);
	const Address & getAddress() const;
	const Address * const getAddressPtr() const;
	int getKey() const;
	void setURL(const std::string & url);
	const std::string getURL() const;
	const std::string getNameURL() const;
	void setName(const std::string & newValue);
	const std::string & getName() const;
	void setEmail(const std::string & newValue);
	const std::string getEmail() const;
	void setType(const std::string & newValue);
	const std::string & getType();
	void setTime(const std::string & newValue);
	void setTime(long newValue);
	long getTime() const;
	bool isAlive() const;
	void setAlive(bool newValue);
	const std::string getTimeAsString() const;
	bool operator==(const PeerEntry & pe) const;

private:
	Address address;
	std::string name;
	std::string type;
	long timestamp = 0;
	bool bAlive = false;
};