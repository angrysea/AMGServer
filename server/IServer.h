#pragma once;

#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <memory>

#include "../socket/PropData.h"

class IServer 
{
public:
	IServer();
	virtual ~IServer();

	static std::shared_ptr<IServer> & getserver();
	static void * getNamedService(const std::string &name);
	static std::string getKey();
	static void setKey();
	static const std::string & getCertificate();
	static void * getController(const std::string &name);

	virtual void initialize(std::map<std::string, std::string> & properties) = 0;
	virtual void initialize(const std::string &configFile) = 0;
	virtual void initialize(...) = 0;
	virtual void start() = 0;
	virtual const std::string & getURL() const = 0;
	virtual int getLocalPort() = 0;
	virtual const std::string & getIdentifier() const = 0;
	virtual const std::string & getInetAddress() const = 0;
	virtual void shutdown() = 0;
	virtual void restart() = 0;
	virtual bool killRequest(short key, bool bForce) = 0;
	virtual bool killRequest(const std::string &key, bool bForce) = 0;
	virtual const std::vector<PropData> & requestList() const = 0;
	virtual void * getAvailableController(const std::string &name) = 0;
	virtual void * getAvailableHandler(const std::string &name) = 0;
	virtual void * getAvailableServlet() = 0;
	virtual void * getAvailableServlet(const std::string &name) = 0;
	virtual void run() = 0;
	virtual void setProperty(const std::string &name, std::string property) = 0;
	virtual const std::string & getProperty(const std::string &name) const = 0;
	virtual int getConnectionTimeOut() const = 0;
	virtual void setConnectionTimeOut(int newValue) = 0;
	virtual const std::string & getHost() const = 0;
	virtual void setHost(const std::string & host) = 0;
	virtual int getPort() const = 0;
	virtual void setPort(int nPort) = 0;
	virtual int getAdminPort() const = 0;
	virtual void setAdminPort(int nPort) = 0;
	virtual int getSecurePort() const = 0;
	virtual void setSecurePort(int nSecurePort) = 0;
	virtual void * getRegistryFile() = 0;
	virtual void * getRegistryDirectory() = 0;
	virtual bool getVerboseFlag() const = 0;
	virtual void setVerboseFlag(bool verbose) = 0;
	virtual void setAutoConnectFlag(bool connect) = 0;
	virtual std::thread getThread() = 0;
	virtual int getClientThreadPriority() const = 0;
	virtual const std::string & getHTTPRoot() const = 0;
	virtual void setHTTPRoot(const std::string & httpRoot) = 0;
	virtual const std::string & getXSLPath() const = 0;
	virtual const std::string & getWebRoot() const = 0;
	virtual const std::string & getClasspath() const = 0;
	virtual void setClasspath(const std::string &classpath) = 0;
	virtual int getMaxClients() const = 0;
	virtual void setMaxClients(int nMaxClients) = 0;
	virtual int getMaxConnections() const = 0;
	virtual void setMaxConnections(int nMaxConnections) = 0;
	virtual const std::string & getLogPath() const = 0;
	virtual void setLogPath(const std::string &path) = 0;
	virtual const std::string & getSocketType() const = 0;
	virtual void setSocketType(const std::string &type) = 0;
	virtual const std::string & getSocketServerClass() const = 0;
	virtual void setSocketServerClass(const std::string &type) = 0;
	virtual const std::map<std::string, std::string> & getConfig() const = 0;
	virtual const std::map<std::string, std::string> & getConfigFromFile() = 0;
	virtual void saveConfig() = 0;
	virtual void saveConfig(const std::map<std::string, std::string> & properties) = 0;
	virtual const std::string & getServletClasspath() const = 0;
	virtual const std::string & getSMTPHost() const = 0;
	virtual void * getFaultTolDBMgr() const = 0;
	virtual bool usesFaultTolerance() const = 0;
	virtual bool useProxy() const = 0;
	virtual const std::string & getProxyAddress() const = 0;
	virtual void * getService(const std::string &name) = 0;
	virtual void * getSetting(const std::string &name) = 0;
	virtual int getMessageCacheSize() const = 0;
	virtual void setMessageCacheSize(int nMessageCacheSize) = 0;

private:
	static std::shared_ptr<IServer> server;
	static std::string key;
	static std::mutex mx;
};
