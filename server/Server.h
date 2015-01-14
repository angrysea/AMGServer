#pragma once

#include <string>

#include "IServer.h"

#include "../controlleragent/PluginAgent.h"
#include "../controlleragent/PluginFactory.h"
#include "../controlleragent/PluginState.h"
#include "../registry/ControllerFile.h"
#include "../socket/BaseSocketServer.h"
#include "../socket/PropData.h"


class Server : IServer 
{
public:
	// Configuration defines
	Server() 
	{
	}

	void initialize(String[] args) throws DBSWSException {

		try {
			startSequence();
			loadSettings(args);
			start();
		} catch (DBSWSException e) {
			throw e;
		} catch (Exception e) {
			ServerException transceiverex = new ServerException(
					DBSWSException.SEVERITY_FATAL,
					ServerException.TCV_INITFAILDED);
			transceiverex.logMessage(e);
			throw transceiverex;
		}
	}

	void initialize(const std::string & configFile) throws DBSWSException {

		try {
			startSequence();
			fileName = configFile;
			if (fileName != null) {
				loadConfig();
			}
			start();
		} catch (DBSWSException e) {
			throw e;
		} catch (Exception e) {
			ServerException transceiverex = new ServerException(
					DBSWSException.SEVERITY_FATAL,
					ServerException.TCV_INITFAILDED);
			transceiverex.logMessage(e);
			throw transceiverex;
		}
	}

	void initialize(Properties properties) throws DBSWSException {

		try {
			startSequence();
			loadConfig(properties);
			start();
		} catch (DBSWSException e) {
			throw e;
		} catch (Exception e) {
			ServerException transceiverex = new ServerException(
					DBSWSException.SEVERITY_FATAL,
					ServerException.TCV_INITFAILDED);
			transceiverex.logMessage(e);
			throw transceiverex;
		}
	}

	void startSequence() throws Exception {

		int start = 0;
		try {
			InetAddress iaddr = InetAddress.getLocalHost();
			host = iaddr.toString();
			if ((start = host.indexOf('/')) > -1) {
				host = host.substring(start + 1);
			}
			identifier = iaddr.getHostName();
		} catch (Exception e) {
			try {
				identifier = "localhost";
				host = InetAddress.getByName("localhost").toString();
				if ((start = host.indexOf('/')) > -1) {
					host = host.substring(start + 1);
				}
			} catch (Exception ee) {
				throw ee;
			}
		}
	}

	void loadSettings(String[] args) throws DBSWSException {
		
		fileName = ServerCommandLine.findConfigFile(args);
		if (fileName != null) {
			loadConfig();
		}
		ServerCommandLine.processCommandLine(args, this);
	}

	void setProperty(const std::string & name, const std::string & property) {
		ServerInfo.properties.setProperty(name, property);
	}

	const std::string & getProperty(const std::string & name) {
		return ServerInfo.properties.getProperty(name);
	}

	void start() throws DBSWSException {

		try {
			IServer.setKey();
			shutdownHookThread = new Thread() {
				void run() {
					try {
						shutdown();
					} catch (Exception e) {
						if (ServerInfo.bVerbose) {
							e.printStackTrace();
						}
					}
				}
			};

			Runtime.getRuntime().addShutdownHook(shutdownHookThread);

			// Create the network agent and do all of the initialization
			networkAgent = new NetworkAgent(this, peerfilename, autoconnect,
					connectType, maxpeers, peerlevels);

			initializeSocketServer();
			intializeFactory();
			controllerFile.preload();
			networkAgent.start();
			pop = MimePop_Plugin.startMailReader();
			
		} catch (DBSWSException e) {
			throw e;
		} catch (Exception e) {
			ServerException transceiverex = new ServerException(
					DBSWSException.SEVERITY_FATAL,
					ServerException.TCV_INITFAILDED);
			transceiverex.logMessage(e);
			throw transceiverex;
		}
	}

	void initializeSocketServer() throws DBSWSException {

		try {
			socketServer = BaseSocketServer.createInstance(socketType,
					socketServerClass);
			socketServer.initialize(this, nPort, nMaxConnections);

			DBSWSException transceiverex = new DBSWSException(
					DBSWSException.SEVERITY_SUCCESS,
					DBSWSException.GEN_MESSAGE);
			transceiverex.logMessage(ServerInfo.VERSION + " starting.");

			socketServer.start(identifier);

			if (nAdminPort > 0) {
				adminSocketServer = BaseSocketServer.createInstance("HTTP");
				adminSocketServer.initialize(this, nAdminPort, nMaxConnections);
				adminSocketServer.start(identifier + "admin");
			}

			if (nSecurePort > 0) {
				secureSocketServer = BaseSocketServer.createInstance("TLS"
						+ socketType);
				secureSocketServer.initialize(this, nSecurePort,
						nMaxConnections);
				secureSocketServer.start(identifier + "SSL");
			}
		} catch (Exception e) {
			ServerException transceiverex = new ServerException(
					DBSWSException.SEVERITY_FATAL,
					ServerException.TCV_INITFAILDED);
			transceiverex.logMessage(e);
			throw transceiverex;
		}

	}

	void intializeFactory() throws DBSWSException {

		try {
			if (controllerfile != null) {
				controllerFile = new PluginFile(controllerfile);
			} else {
				controllerFile = new PluginFile();
			}

			controllerFactory = new PluginFactory(classpath, verbose);
			controllerFactory.initialize(this, nMaxClients);
		} catch (Exception e) {
			ServerException transceiverex = new ServerException(
					DBSWSException.SEVERITY_FATAL,
					ServerException.TCV_INITFAILDED);
			transceiverex.logMessage(e);
			throw transceiverex;
		}
	}

	boolean killRequest(const std::string & id, boolean force) {
		return controllerFactory.killPlugin(id, force);
	}

	boolean killRequest(short id, boolean force) {
		return false;
	}

	Vector<PropData> requestList() throws DBSWSException {

		Map<String, PluginState> clientList = controllerFactory.getPluginAgents();
		Vector<PropData> vec = new Vector<PropData>(32);

		if (clientList != null) {
			String[] stat_arr = { "Idle", "Busy", "Free", "Killed", "Finished" };
			Iterator<PluginState> it = clientList.values().iterator();

			try {
				while (it.hasNext()) {
					PluginState state = it.next();
					PluginAgent controller = state.getPluginAgent();

					const std::string & controllerName = controller.getName();
					if (controllerName == null) {
						controllerName = new String("no controller");
					}

					PropData props = new PropData(controllerName, Integer
							.toString(state.getId()), stat_arr[state
							.getStatus()]);

					vec.add(props);
				}
			} catch (Exception e) {
				ServerException transceiverex = new ServerException(
						DBSWSException.SEVERITY_FATAL,
						ServerException.TCV_INITFAILDED);
				transceiverex.logMessage(e);
				throw transceiverex;
			}
		}

		return vec;
	}

	URL getURL() throws DBSWSException {

		if (url == null) {
			try {
				if (nPort != 80) {
					url = new URL("http", host, nPort, "/");
				} else {
					url = new URL("http", host, "/");
				}
			} catch (Exception e) {
				ServerException transceiverex = new ServerException(
						DBSWSException.SEVERITY_FATAL,
						ServerException.TCV_URLFAILDED);
				transceiverex.logMessage(e);
				throw transceiverex;
			}
		}
		return url;
	}

	synchronized void restart() throws DBSWSException {

		try {
			shutdown(true);
		} catch (DBSWSException e) {
			throw e;
		}
	}

	synchronized void shutdown() throws DBSWSException {

		try {
			shutdown(false);
		} catch (DBSWSException e) {
			throw e;
		}
	}

	synchronized void shutdown(boolean restart)
			throws DBSWSException {

		// Let the network drop out.
		try {
			networkAgent.disconnect();
		} catch (Exception e) {
		}

		boolean bAbruptShutdown = shutdownHookThread != null
				&& shutdownHookThread.isAlive();
		try {
			if (pop != null) {
				pop.stop();
			}
		} catch (Exception e) {
		}

		if (!bAbruptShutdown) {
			try {
				Runtime.getRuntime().removeShutdownHook(shutdownHookThread);
			} catch (Exception e) {
			}
		} else {
			ServerException se = new ServerException(
					ServerException.SEVERITY_WARNING,
					ServerException.TCV_ABRUPTSHUTDOWN);
			se.logMessage("It is recommended that the Server be shut down from SWS Administration pages. "
							+ "Failure to do so could result in the loss of system resources.");
		}

		bFinishing = true;
		bRestarting = restart;

		try {
			socketServer.shutdown();
			if (adminSocketServer != null) {
				adminSocketServer.shutdown();
			}
			if (secureSocketServer != null) {
				secureSocketServer.shutdown();
			}

			controllerFile.closeFile();
			networkAgent.closeFile();

			try {
				socketServer.join(10000);
			} catch (Exception ex) {
				ex.printStackTrace();
			}

			if (!restart) {
				// If shutdown occurred abruptly, calling exit will cause a
				// deadlock
				if (!bAbruptShutdown) {
					System.exit(0);
				}
			}
		} catch (Exception e) {
			ServerException transceiverex = new ServerException(
					DBSWSException.SEVERITY_FATAL, 999);
			e.printStackTrace();
			throw transceiverex;
		}
	}

	void cleanup(boolean restart) throws DBSWSException {

		try {
			if (socketServer != null) {
				socketServer.shutdown();
			}
			socketServer = null;

			if (adminSocketServer != null) {
				adminSocketServer.shutdown();
			}
			adminSocketServer = null;

			if (secureSocketServer != null) {
				secureSocketServer.shutdown();
			}
			secureSocketServer = null;

			bRestarting = false;
			bFinishing = false;
		} catch (Exception e) {
			System.out.println(e.getMessage() + " In cleanup");
		}

		if (restart) {
			try {
				loadConfig();
				start();
			} catch (DBSWSException e) {
				throw e;
			} catch (Exception e) {
				ServerException transceiverex = new ServerException(
						DBSWSException.SEVERITY_FATAL, 999);
				transceiverex.logMessage(e);
				throw transceiverex;
			}
		}
	}

	void run(Runnable runner) {
		try {
			if (runner instanceof PluginAgent) {
				controllerFactory.run((PluginAgent) runner);
			}
		} catch (ClassCastException cce) { // highly unlikely...
			ServerException xse = new ServerException(
					DBSWSException.SEVERITY_ERROR,
					DBSWSException.GEN_TYPEMISMATCH, cce.getMessage());
			xse.logMessage();
		}
	}

	void saveConfig() {
		try {
			Properties serverProps = getConfig();
			serverProps.store(new java.io.FileOutputStream(fileName), fileName);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	void saveConfig(Properties serverProps) {
		try {
			serverProps.store(new java.io.FileOutputStream(fileName), fileName);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	void loadConfig() {

		try {
			ServerInfo.properties = ServerProperties
					.getInstance("com.db.sws.transceiver.SimpleProperties");
			ServerInfo.properties.load(new java.io.FileInputStream(
					fileName));
			loadConfig(ServerInfo.properties);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	void loadConfig(Properties properties) {

		try {
			const std::string & s = null;
			socketType = properties.getProperty(ServerConfig.TYPE,
					"Controller");
			if (socketType.equals("CUSTOM")) {
				socketServerClass = properties
						.getProperty(ServerConfig.SOCKETSERVERCLASS);
			} else {
				socketServerClass = null;
			}
			nMaxConnections = Integer.parseInt(properties.getProperty(
					ServerConfig.MAX_CONNECTIONS, "30"));
			nPort = Integer.parseInt(properties.getProperty(
					ServerConfig.PORT, "0"));
			nAdminPort = Integer.parseInt(properties.getProperty(
					ServerConfig.ADMINPORT, "0"));
			maxpeers = Integer.parseInt(properties.getProperty(
					ServerConfig.MAXPEERS, "0"));
			peerlevels = Integer.parseInt(properties.getProperty(
					ServerConfig.LEVELS, "0"));

			nMaxClients = Integer.parseInt(properties.getProperty(
					ServerConfig.MAX_CLIENTS, "30"));
			nSecurePort = Integer.parseInt(properties.getProperty(
					ServerConfig.SECUREPORT, "1443"));

			timeout = Integer.parseInt(properties.getProperty(
					ServerConfig.CONNECTION_TIMEOUT, "0"));
			timeout *= 1000;

			classpath = properties.getProperty(ServerConfig.CLASSPATH);

			s = properties.getProperty(ServerConfig.VERBOSE);
			if (s != null && s.equals(TRUE)) {
				verbose = true;
			} else {
				verbose = false;
			}

			s = properties.getProperty(ServerConfig.AUTOCONNECT);
			if (s != null && s.equals(TRUE)) {
				autoconnect = true;
			} else {
				autoconnect = false;
			}

			s = properties.getProperty(ServerConfig.SHOWCONSOLE, FALSE);
			if (s != null && s.equals(TRUE)) {
				showconsole = true;
			} else {
				showconsole = false;
			}

			s = properties.getProperty(ServerConfig.USEPROXY, FALSE);
			if (s != null && s.equals(TRUE)) {
				bUseProxy = true;
			} else {
				bUseProxy = false;
			}

			httpRoot = properties.getProperty(ServerConfig.HTTP_ROOT);
			webRoot = properties.getProperty(ServerConfig.WEB_ROOT);
			peerfilename = properties.getProperty(ServerConfig.PEER_FILE);
			connectType = properties.getProperty(ServerConfig.CONNECTTYPE);
			controllerfile = properties.getProperty(ServerConfig.PLUGIN_FILE);
			webRoot = properties.getProperty(ServerConfig.WEB_ROOT);
			httpRoot = properties.getProperty(ServerConfig.HTTP_ROOT, ".");
			SMTPHost = properties.getProperty(ServerConfig.SMTPHOST,
					"localhost");
			host = properties.getProperty(ServerConfig.HOST, host);
			proxyAddress = properties.getProperty(
					ServerConfig.PROXYADDRESS, proxyAddress);

			nSecurePort = Integer.parseInt(properties.getProperty(
					ServerConfig.SECUREPORT, "0"));
			if (nSecurePort > 0) {
				keyStore = properties.getProperty(ServerConfig.KEY_STORE,
						"KeyStore");
				keyStorePass = properties.getProperty(
						ServerConfig.KEY_STORE_PASSPHRASE, "seamaster");
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	const std::string & getSMTPHost() {
		return SMTPHost;
	}

	Properties getConfigFromFile() {
		Properties serverProps = new Properties();

		try {
			serverProps.load(new java.io.FileInputStream(fileName));
		} catch (Exception e) {
			e.printStackTrace();
		}
		return serverProps;
	}

	Properties getConfig() {
		ServerInfo.properties = ServerProperties
				.getInstance("com.db.sws.transceiver.SimpleProperties");

		try {
			ServerInfo.properties.setProperty(ServerConfig.HTTP_ROOT,
					httpRoot);
			ServerInfo.properties.setProperty(ServerConfig.CLASSPATH,
					classpath);
			ServerInfo.properties.setProperty(ServerConfig.WEB_ROOT,
					webRoot);
			ServerInfo.properties.setProperty(ServerConfig.SMTPHOST,
					SMTPHost);
			ServerInfo.properties.setProperty(
					ServerConfig.PROXYADDRESS, proxyAddress);
			ServerInfo.properties
					.setProperty(ServerConfig.HOST, host);

			ServerInfo.properties.setProperty(
					ServerConfig.MAX_CLIENTS, Integer
							.toString(nMaxClients));
			ServerInfo.properties.setProperty(
					ServerConfig.MAX_CONNECTIONS, Integer
							.toString(nMaxConnections));
			ServerInfo.properties.setProperty(ServerConfig.MAXPEERS,
					Integer.toString(maxpeers));
			ServerInfo.properties.setProperty(ServerConfig.LEVELS,
					Integer.toString(peerlevels));
			ServerInfo.properties.setProperty(ServerConfig.PORT,
					Integer.toString(nPort));
			ServerInfo.properties.setProperty(ServerConfig.TYPE,
					socketType);
			ServerInfo.properties.setProperty(
					ServerConfig.SOCKETSERVERCLASS, socketServerClass);

			if (timeout > 0) {
				ServerInfo.properties.setProperty(
						ServerConfig.CONNECTION_TIMEOUT, Integer
								.toString(timeout / 1000));
			}
			if (nSecurePort > 0) {
				ServerInfo.properties.setProperty(
						ServerConfig.SECUREPORT, Integer
								.toString(nSecurePort));
			}
			if (nAdminPort > 0) {
				ServerInfo.properties.setProperty(
						ServerConfig.ADMINPORT, Integer
								.toString(nAdminPort));
			}
			if (controllerFile != null) {
				ServerInfo.properties.setProperty(
						ServerConfig.PLUGIN_FILE, controllerFile.getName());
			}
			if (networkAgent != null) {
				ServerInfo.properties.setProperty(
						ServerConfig.PEER_FILE, networkAgent.getName());
			}
			if (verbose == true) {
				ServerInfo.properties.setProperty(
						ServerConfig.VERBOSE, TRUE);
			} else {
				ServerInfo.properties.setProperty(
						ServerConfig.VERBOSE, FALSE);
			}
			if (autoconnect == true) {
				ServerInfo.properties.setProperty(
						ServerConfig.AUTOCONNECT, TRUE);
			} else {
				ServerInfo.properties.setProperty(
						ServerConfig.AUTOCONNECT, FALSE);
			}
			if (bUseProxy == true) {
				ServerInfo.properties.setProperty(
						ServerConfig.USEPROXY, TRUE);
			} else {
				ServerInfo.properties.setProperty(
						ServerConfig.USEPROXY, TRUE);
			}
			if (showconsole == true) {
				ServerInfo.properties.setProperty(
						ServerConfig.SHOWCONSOLE, TRUE);
			} else {
				ServerInfo.properties.setProperty(
						ServerConfig.SHOWCONSOLE, FALSE);
			}
			if (nSecurePort > 0) {
				ServerInfo.properties.setProperty(
						ServerConfig.KEY_STORE, keyStore);
				ServerInfo.properties.setProperty(
						ServerConfig.KEY_STORE_PASSPHRASE, keyStorePass);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		return ServerInfo.properties;
	}

	Object getAvailablePlugin(const std::string & name) {
		return controllerFactory.getAvailablePlugin(name);
	}

	Object getAvailableHandler(const std::string & name) {
		return null;
	}

	Object getAvailableBroker(const std::string & name) {
		return null;
	}

	Object getAvailableServlet() {
		return null;
	}

	Object getAvailableServlet(const std::string & name) {
		return null;
	}

	const std::string & getIdentifier() {
		return identifier;
	}

	int getLocalPort() {
		return socketServer.getLocalPort();
	}

	int getSecureLocalPort() {
		return secureSocketServer.getLocalPort();
	}

	int getAdminLocalPort() {
		return adminSocketServer.getLocalPort();
	}

	InetAddress getInetAddress() {
		return socketServer.getInetAddress();
	}

	InetAddress getAdminInetAddress() {
		return adminSocketServer.getInetAddress();
	}

	InetAddress getSecureInetAddress() {
		return secureSocketServer.getInetAddress();
	}

	final int getConnectionTimeOut() {
		return timeout;
	}

	final void setConnectionTimeOut(int newValue) {
		timeout = newValue;
	}

	final const std::string & getHost() {
		return host;
	}

	final const std::string & getProxyAddress() {
		return proxyAddress;
	}

	final void setHost(const std::string & host) {
		this.host = host;
	}

	final int getPort() {
		return nPort;
	}

	final void setPort(int nPort) {
		this.nPort = nPort;
	}

	final const std::string & getSocketType() {
		return socketType;
	}

	final void setSocketType(const std::string & socketType) {
		this.socketType = socketType;
	}

	final const std::string & getSocketServerClass() {
		return socketServerClass;
	}

	final void setSocketServerClass(const std::string & socketServerClass) {
		this.socketServerClass = socketServerClass;
	}

	final int getAdminPort() {
		return nAdminPort;
	}

	final void setAdminPort(int nAdminPort) {
		this.nAdminPort = nAdminPort;
	}

	final int getSecurePort() {
		return nSecurePort;
	}

	final int getMessageCacheSize() {
		return messageCacheSize;
	}

	final void setMessageCacheSize(int nMessageCacheSize) {
		this.messageCacheSize = nMessageCacheSize;
	}
		
	final void setSecurePort(int nSecurePort) {
		this.nSecurePort = nSecurePort;
	}

	Object getRegistryFile() {
		return null;
	}

	Object getRegistryDirectory() {
		return null;
	}

	final boolean getVerboseFlag() {
		return verbose;
	}

	final void setVerboseFlag(boolean verbose) {
		this.verbose = verbose;
	}

	final boolean getAutoConnectFlag() {
		return autoconnect;
	}

	final void setAutoConnectFlag(boolean autoconnect) {
		this.autoconnect = autoconnect;
	}

	final Thread getThread() {
		return null; // thread;
	}

	final int getClientThreadPriority() {
		return clientPriority;
	}

	final const std::string & getHTTPRoot() {
		if (httpRoot != null && !httpRoot.endsWith(File.separator)) {
			httpRoot += File.separator;
		}
		return httpRoot;
	}

	final void setHTTPRoot(const std::string & httpRoot) {
		this.httpRoot = httpRoot;
	}

	final const std::string & getWebRoot() {
		return this.webRoot;
	}

	final const std::string & getClasspath() {
		return classpath;
	}

	const std::string & getServletClasspath() {
		return null;
	}

	final void setClasspath(const std::string & classpath) {
		this.classpath = classpath;
	}

	final int getMaxClients() {
		return nMaxClients;
	}

	final void setMaxClients(int nMaxClients) {
		this.nMaxClients = nMaxClients;
	}

	final int getMaxConnections() {
		return nMaxConnections;
	}

	final void setMaxConnections(int nMaxConnections) {
		this.nMaxConnections = nMaxConnections;
	}

	const std::string & getXSLPath() {
		return null;
	}

	Object getFaultTolDBMgr() {
		return null;
	}

	final boolean usesFaultTolerance() {
		return false;
	}

	final boolean useProxy() {
		return bUseProxy;
	}

	Object getSetting(const std::string & name) {
		if (name.equalsIgnoreCase(ServerConfig.SHOWCONSOLE)) {
			return new Boolean(showconsole);
		}
		return null;
	}

	Object getService(const std::string & name) {

		if (name.equalsIgnoreCase(ServerConfig.NETWORKAGENT)) {
			return networkAgent;
		} else if (name.equalsIgnoreCase(ServerConfig.PEER_FILE)) {
			return peerfilename;
		} else if (name.equalsIgnoreCase(ServerConfig.PLUGIN_FILE)) {
			return controllerFile;
		} else if (name.equalsIgnoreCase(ServerConfig.PLUGINFACTORY)) {
			return controllerFactory;
		}
		return null;
	}

	void setLogPath(const std::string & path) {
	}

	const std::string & getLogPath() {
		return null;
	}

	static void main(const std::string & args[]) {

		Server transceiver = null;

		try {
			do {
				System.out.println(args[0]);
				try {
					transceiver = new Server();
				} catch (Exception e) {
					e.printStackTrace();
					System.exit(1);
				}

				transceiver.initialize(args);
				System.out.println(ServerInfo.VERSION
						+ " listening on port [" + transceiver.nPort
						+ "] started...");
				if (transceiver.getAdminPort() > 0) {
					System.out.println("Administration listening on port ["
							+ transceiver.nAdminPort + "], web serving from ["
							+ transceiver.getWebRoot() + "]...");
				}
			} while (transceiver.bRestarting == true);
		} catch (Exception e) {
			try {
				if (transceiver != null) {
					transceiver.cleanup(false);
				}
			} catch (DBSWSException ce) {
				ce.printStackTrace();
			}
			System.err.println(ServerInfo.VERSION + " is shutting down..."
					+ "Exception=[" + e.getMessage() + "]");
			System.exit(1);
		}
	}

public:
	static final const std::string & TRUE = "true";
	static final const std::string & FALSE = "false";

protected:
	static final const std::string & PROP_ROOT_DIR = "root";
	static final boolean REDIRECT_OUTPUT_STREAMS = false;

	int clientPriority = Thread.NORM_PRIORITY;
	boolean bUseProxy = false;
	BaseSocketServer socketServer = null;
	BaseSocketServer adminSocketServer = null;
	BaseSocketServer secureSocketServer = null;
	PluginFactory controllerFactory = null;
	boolean bRestarting = false;
	boolean bFinishing = false;
	int nPort = 8082;
	int nAdminPort = 0;
	int nSecurePort = 0;
	const std::string & host = "localhost";
	const std::string & classpath = null;
	const std::string & identifier = "Server";
	URL url = null;
	int nMaxClients = 0;
	int maxpeers = 10;
	int peerlevels = 4;
	int nMaxConnections = 0;
	boolean verbose = false;
	boolean autoconnect = false;
	boolean showconsole = false;
	PluginFile controllerFile = null;
	NetworkAgent networkAgent = null;
	int timeout = 0;
	const std::string & fileName = null;
	const std::string & peerfilename = null;
	const std::string & connectType = null;
	const std::string & controllerfile = null;
	const std::string & httpRoot = null;
	const std::string & webRoot = null;
	const std::string & SMTPHost = null;
	const std::string & proxyAddress = null;
	const std::string & keyStore = null;
	const std::string & keyStorePass = null;
	const std::string & socketType = null;
	const std::string & socketServerClass = null;
	int messageCacheSize = 5000;

	ServerCommandLine transceiverCmdLine = null;
	Thread shutdownHookThread;
	MimePop_Plugin pop = null;

};
