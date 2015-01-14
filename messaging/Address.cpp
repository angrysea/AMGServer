#include "Address.h"
#include "EqualsIgnoreCase.h"

Address Address::address("test.com", 1024);

Address::Address()
{
	computeHashCode();
}

/**
* Constucts an Address object representing the given URI
*
* @param uri
*            URI that this object represents
*/
Address::Address(const std::string & uri)
{
	if (!uri.empty())
		setURI(uri);
}

/**
* Constructs an Address that is a copy of the specified address
*
* @param address
*            Address to use for initialization
*/
Address::Address(const Address & address) :
preffix{ address.preffix },
postfix{ address.postfix },
host{ address.host },
port{ address.port },
name{ address.name },
controller{ address.controller },
method{ address.method },
email{ address.email },
type{ address.type },
hashcode{ address.hashcode }
{
}

/**
* Constucts an Address representing the URI of the specified transceiver
*
* @param transceiver
*            Transceiver to use for initialization
* @param bSecure
*            true to use the https protocol. false to use the http
*            protocol.
*/
Address::Address(const std::string & host, int nPort, bool bSecure)
{
	if (bSecure == false)
		preffix = "http";
	else
		preffix = "https";

	setHost(host);
	port = std::to_string(nPort);
	computeHashCode();
}

/**
* Constucts an Address representing the URI of the specified transceiver
* using the default (http) protocol.
*
* @param transceiver
*            Transceiver to use for initialization
*/
Address::Address(const std::string & host, int nPort) :
Address{ host, nPort, false }
{
}

Address & Address::operator=(const Address & address)
{
	preffix = address.preffix;
	postfix = address.postfix;
	host = address.host;
	port = address.port;
	name = address.name;
	controller = address.controller;
	method = address.method;
	email = address.email;
	type = address.type;
	hashcode = address.hashcode;
	return *this;
}

const std::string Address::tostring() const
{
	std::string sb;
	sb.append(preffix);
	sb.append(":");
	sb.append(postfix);
	sb.append(":");
	sb.append(host);
	sb.append(":");
	sb.append(port);
	sb.append("/");
	sb.append(controller);
	sb.append("/");
	sb.append(method);
	sb.append("/");
	sb.append(type);
	sb.append("/");
	sb.append(name);
	return sb;
}


/**
* Retrieves the URL this object represents
*
* @return URL of this object
*/
const std::string Address::getURL() const
{
	std::string uri;
	if (!preffix.empty() && !host.empty())
	{
		uri.append(preffix);
		uri.append("://");
		uri.append(host);
		uri.append(":");
		if (port.empty())
			uri.append("8082");
		else
			uri.append(port);
	}
	return uri;
}

/**
* Retrieves the URL this object represents
*
* @return URL of this object
*/
const std::string Address::getMyURL()
{
	return address.getURL();
}

/**
* Retrieves the URL with Host Name this object represents
*
* @return URL of this object
*/
const std::string Address::getNameURL() const
{
	std::string uri;
	if (!preffix.empty() && !host.empty())
	{
		uri.append(preffix);
		uri.append("://");
		uri.append(name);
		uri.append(":");
		if (port.empty())
			uri.append("8082");
		else
			uri.append(port);
	}
	return uri;
}

/**
* Retrieves the URI this object represents
*
* @return URI of this object
*/
const std::string Address::getURI() const
{
	std::string uri;
	if (!preffix.empty() && !host.empty())
	{
		uri.append(preffix);
		uri.append("://");
		uri.append(host);
		uri.append(":");

		if (!port.empty())
			uri.append(port);
		else
			uri.append("8082");

		if (!controller.empty())
		{
			uri.append("/");
			uri.append(controller);
			if (!method.empty())
			{
				uri.append("/");
				uri.append(method);
			}
		}
	}
	return uri;
}

/**
* Initializes this object with the specified URI
*
* @param uri
*            source URI
*/
void Address::setURI(const std::string & uri)
{
	int start = 0;

	if (uri.find("https://") == 0)
	{
		start = 8;
		preffix = "https";
	}
	else
	{
		preffix = "http";
		if (uri.find("http://") == 0)
			start = 7;
		else
			start = 0;
	}

	int next = uri.find(":", start);
	if (next != std::string::npos)
	{
		setHost(uri.substr(start, next - start));
		start = next + 1;
		next = uri.find("/", start);
		if (next != std::string::npos)
		{
			port = uri.substr(start, next - start);
			start = next + 1;
			next = uri.substr(start).find("/");
			if (next != std::string::npos)
			{
				controller = uri.substr(start, next - start);
				start = next + 1;
				next = uri.substr(start).find("/");
				if (next != std::string::npos)
				{
					method = uri.substr(start, next - start);
				}
			}
		}
		else
		{
			port = uri.substr(start);
		}
	}
	else
	{
		if (uri.find("/") == 0)
		{
			start = 1;
		}
		host = "broadcast";
		port = "0";
		next = 0;
	}
	computeHashCode();
}

/**
* Initializes this object with the specified URI
*
* @param url
*            source URL
*/
void Address::setURL(const std::string & url)
{
	int start = 0;
	if (url.find("https://") == 0)
	{
		start = 8;
		preffix = "https";
	}
	else
	{
		preffix = "http";
		if (url.find("http://"))
		{
			start = 7;
		}
	}

	int next = url.find(":", start);
	if (next != std::string::npos)
	{
		setHost(url.substr(start, next - start));
		start = next + 1;
	}

	next = url.find("/", start);
	if (next != std::string::npos)
	{
		port = url.substr(start);
	}
	else
	{
		port = url.substr(start, next - start);
	}
	computeHashCode();
}

/**
* Determines whether this Address is using a secure protocol.
*
* @return true if a secure protocol is in use, otherwise false
*/
bool Address::isSecure() const
{
	return preffix == "https";
}

/**
* Sets the security protocol for this address.
*
* @param bSecure
*            true to use https protocol, false to use the http protocol.
*/
void Address::setSecure(bool bSecure)
{
	if (bSecure == false)
		preffix = "http";
	else
		preffix = "https";
}

/**
* Sets the preffix to use for this Address
*
* @param preffix
*            preffix to set
*/
void Address::setPrefix(const std::string & newValue)
{
	preffix = newValue;
}

/**
* Retrieves the preffix used by this Address
*
* @return preffix used
*/
const std::string Address::getPrefix() const
{
	return preffix;
}

/**
* Sets the postfix to use for this Address
*
* @param postfix
*            postfix to set
*/
void Address::setPostfix(const std::string & newValue)
{
	postfix = newValue;
}

/**
* Retrieves the postfix used by this Address
*
* @return postfix used
*/
const std::string Address::getPostfix() const
{
	return postfix;
}

/**
* Sets the host name or IP address of this Address
*
* @param host
*            host to set
*/
void Address::setHost(const std::string & host)
{
	changeHostToIP(host);
	computeHashCode();
}

/**
* Retrieves the host name or IP of this Address
*
* @return host IP address
*/
const std::string Address::getHost() const
{
	return host;
}

/**
* Retrieves the host name or IP of this Address
*
* @return host IP address
*/
const std::string Address::getMyHost()
{
	return address.getHost();
}

/**
* Retrieves the host name or IP of this Address
*
* @return host name address
*/
const std::string Address::getHostByName() const
{
	return name;
}

/**
* Sets the port number of this Address
*
* @param port
*            port to set
*/
void Address::setPort(const std::string & newValue)
{
	port = newValue;
	computeHashCode();
}

/**
* Retrieves the port used by this Address
*
* @return port used by this Address
*/
const std::string Address::getPort() const
{
	return port;
}

/**
* Retrieves the port used by this Address
*
* @return port used by this Address
*/
std::string Address::getMyPort()
{
	return address.getPort();
}

/**
* Sets the controller name of this Address
*
* @param controller
*            name of controller
*/
void Address::setController(const std::string & newValue)
{
	controller = newValue;
}

/**
* Retrieves the name of the controller for this Address
*
* @return name of controller
*/
const std::string Address::getController() const
{
	return controller;
}

/**
* Sets the method name used for this Address
*
* @param method
*            method name
*/
void Address::setMethod(const std::string & newValue)
{
	method = newValue;
}

/**
* Retrieves the method name for this Address
*
* @return method name
*/
const std::string Address::getMethod() const
{
	return method;
}

/**
* Sets the type for this Address
*
* @param type
*            type to set
*/
void Address::setType(const std::string & newValue)
{
	type = newValue;
}

/**
* Retrieves the type for this Address
*
* @return type
*/
const std::string Address::getType() const
{
	return type;
}

/**
* Sets the email name used for this Address
*
* @param email
*            email name
*/
void Address::setEmail(const std::string & newValue)
{
	email = newValue;
}

/**
* Retrieves the email name for this Address
*
* @return email name
*/
const std::string Address::getEmail() const
{
	return email;
}

/**
* Returns the hash code.
*
* @return hash code
*/
int Address::hashCode() const
{
	return hashcode;
}

/**
* Locks the hash code for this object
*/
void Address::lock()
{
	bLocked = true;
}

/**
* Unlocks the hash code for this object
*/
void Address::unlock()
{
	bLocked = false;
}

/**
* Computes a new hash code based on the attributes set on this object
*/
void Address::computeHashCode()
{
	if (bLocked == false)
	{
		std::hash<std::string> hash_func;
		hashcode = hash_func(host + port);
		//System.out.println("");
		//System.out.println("Adress = " +host + port);
		//System.out.println("hashcode = " + hashcode);
		//System.out.println("");
		//System.out.println("");
	}
}

bool Address::operator==(const Address & address) const
{
	return hashcode == address.hashCode();
}

bool Address::operator!=(const Address & address) const
{
	return hashcode != address.hashCode();
}

/**
* Checks the host name to see if it is an IP address or not if it is not it
* converts the name to an IP address.
*/
void Address::changeHostToIP(const std::string & name)
{
	// Check to see if the host is already an IP address.
	int len = name.length();

	const char * data = name.c_str();
	bool lastdot = false;
	for (int i = 0; i < len; i++)
	{
		if (data[i] == '.')
		{
			// If this is true there is come kind of error
			// but try the conversion
			if (lastdot == true)
				return;
		}
		else if (isdigit(data[i]) == false)
		{
			changeToIP(name);
			return;
		}
	}

	// If we have dropped down to here and there are
	// we can't be sure lets excpet an ip
	changeToName(name);
}

/**
* Utility function used by changeHostToIP
*/
void Address::changeToName(const std::string & ip)
{
	host = ip;
	name = ip;
}

bool Address::empty() const
{
	return preffix.empty()
		&& postfix.empty()
		&& host.empty()
		&& port.empty()
		&& controller.empty()
		&& method.empty()
		&& type.empty()
		&& name.empty()
		&& email.empty();
}

/**
* Utility function used by changeHostToIP
*/
void Address::changeToIP(const std::string & newIP)
{
	//host = InetAddress.getByName(name);
	int start = 0;
	if ((start = host.find('/')) != std::string::npos)
		host = host.substr(start + 1);
	name = newIP;
}

/**
* If the peer sits behind a firewall all messages will have to be routed
* through. This will allow another peer to post messages through the
* firewall utility.
*/
const Address & Address::getRoute() const
{
	if (!route->empty() && route->lastStop() == false)
		return route->getRoute();
	return *route;
}

void Address::setRoute(const Address & newValue)
{
	if (route != nullptr)
		route->setRoute(newValue);
	*route = newValue;
}

const Address Address::hop() const
{
	Address hop;
	if (!route->empty())
	{
		if (route->lastStop())
		{
			hop = *route;
		}
		else
		{
			hop = route->hop();
		}
	}
	return hop;
}

bool Address::lastStop() const
{
	return (route->empty());
}

void Address::setSync()
{
	type = "sync";
}

bool Address::isSync() const
{
	std::string data;
	return (!type.empty() && equalsIgnoreCase(data, "sync"));
}
