#pragma once

#include <string>
#include <memory>

/**
* The Address class represents a source or destination URL of a peer.
*/
class Address
{
public:
	Address();
	Address(const std::string & uri);
	Address(const Address & address);
	Address(const std::string & host, int nPort);
	Address(const std::string & host, int nPort, bool bSecure);

	Address & operator=(const Address & address);
	const std::string tostring() const;
	const std::string getURL() const;
	static const std::string getMyURL();
	const std::string getNameURL() const;
	const std::string getURI() const;
	void setURI(const std::string & uri);
	void setURL(const std::string & url);
	bool isSecure() const;
	void setSecure(bool bSecure);
	void setPrefix(const std::string & newValue);
	const std::string getPrefix() const;
	void setPostfix(const std::string & newValue);
	const std::string getPostfix() const;
	void setHost(const std::string & host);
	const std::string getHost() const;
	static const std::string getMyHost();
	const std::string getHostByName() const;
	void setPort(const std::string & newValue);
	const std::string getPort() const;
	static std::string getMyPort();
	void setController(const std::string & newValue);
	const std::string getController() const;
	void setMethod(const std::string & newValue);
	const std::string getMethod() const;
	void setType(const std::string & newValue);
	const std::string getType() const;
	void setEmail(const std::string & newValue);
	const std::string getEmail() const;
	int hashCode() const;
	void lock();
	void unlock();
	void computeHashCode();
	bool operator==(const Address & address) const;
	bool operator!=(const Address & address) const;
	void changeHostToIP(const std::string & name);
	void changeToName(const std::string & ip);
	bool empty() const;
	void changeToIP(const std::string & newIP);
	const Address & getRoute() const;
	void setRoute(const Address & newValue);
	const Address hop() const;
	bool lastStop() const;
	void setSync();
	bool isSync() const;

private:
	std::unique_ptr<Address> route;
	std::string preffix;
	std::string postfix;
	std::string host;
	std::string port;
	std::string controller;
	std::string method;
	std::string type;
	std::string name;
	std::string email;
	int hashcode = 0;
	bool bLocked = false;
	static Address address;
};
