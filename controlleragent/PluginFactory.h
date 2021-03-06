package com.db.sws.pluginagent;

import java.util.Collections;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.dbswsex.PluginException;
import com.db.sws.messaging.Messenger;
import com.db.sws.registry.PluginEntry;
import com.db.sws.registry.PluginFile;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiverutils.CachedThread;
import com.db.sws.transceiverutils.ThreadCache;


public class PluginFactory implements Runnable {

	static public final String DB = "dbsws";
	static public final String DBCLASS = "com.db.sws.pluginagent.DBPlugin";

	static public final String MAINTENANCE = "dbm";
	static public final String MAINTENANCECLASS = "com.db.sws.pluginagent.MaintenancePlugin";

	static public final String SERVICE = "service";
	static public final String SERVICECLASS = "com.db.sws.pluginagent.ServicePlugin";

	static public final String CACHE = "cache";
	static public final String CACHECLASS = "com.db.sws.cache.CachePlugin";
	
	static public final String CONSOLE = "Console";
	static public final String CONSOLECLASS = "com.db.sws.plugins.Console";

	private ThreadCache threadcache = null;
	private Map<String, PluginState> plugins = null;
	private ITransceiver transceiver = null;
	private boolean verbose = false;
	private PluginFile pluginFile = null;
	@SuppressWarnings("unused")
	private boolean running = true;
	@SuppressWarnings("unused")
	private String classpath = null;

	public PluginFactory(String classpath, boolean v) {
		verbose = v;
		this.classpath = classpath;
		plugins = (Map<String, PluginState>) Collections
				.synchronizedMap(new HashMap<String, PluginState>());
		try {
			Class.forName("com.db.sws.pluginagent.Plugin");
		} catch (Exception e) {
		}
	}

	protected synchronized void deleteClient(PluginState state) {
		plugins.remove(state);
	}

	protected synchronized PluginState addPlugin(String name) {
		PluginState state = null;

		try {
			CachedThread t = threadcache.getThread(true);
			PluginEntry entry = null;
			PluginAgent plugin = new PluginAgent(this, t, verbose);
			plugin.setName(name);
			state = plugin.getState();
			if ((entry = pluginFile.findEntry(name)) != null) {
				state.appendClasspath(entry.getClasspath());
				plugin.setEntry(entry);
			}
			state.setStatus(PluginState.FREE);
			plugins.put(name, state);
		} catch (Exception e) {
			state = null;
		}

		return state;
	}

	public void initialize(ITransceiver theServer, int nplugins) {
		transceiver = theServer;
		pluginFile = (PluginFile) transceiver.getService("pluginfile");
		threadcache = new ThreadCache("plugins", nplugins, transceiver
				.getClientThreadPriority(), 0);
	}

	public void run() {
		while (true) {
			if (Thread.interrupted() == true) {
				return;
			}
			
			killTimedoutPlugins();

			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				return;
			}
		}
	}

	synchronized public void postMessage(String method, Object args[]) {

		Iterator<PluginState> it = plugins.values().iterator();
		while (it.hasNext()) {
			PluginState state = it.next();
			Messenger.localPostMessage(state.getPluginAgent(), method, args);
		}
	}

	synchronized protected void killTimedoutPlugins() {
		try {
			Iterator<PluginState> it = plugins.values().iterator();

			while (it.hasNext()) {
				stopThisPlugin(it.next());
			}
		} catch (Exception e) {
		}
	}

	public void peerUpdate() {
		Iterator<PluginState> it = plugins.values().iterator();
		while (it.hasNext()) {
			/**
			 * Will need to look into this for now just suppress the warning
			 */
			@SuppressWarnings("unused")
			PluginState state = it.next();
		}
	}

	final public boolean killPlugin(String name, boolean force) {
		return stopPlugin(name);
	}

	final public boolean stopPlugin(String id) {
		boolean bStop = true;
		PluginState state = plugins.get(id);
		if (state != null) {
			bStop = false;
		}

		while (bStop == false) {
			bStop = stopThisPlugin(state);
			try {
				Thread.sleep(state.getTimeOut());
			} catch (InterruptedException e) {
				bStop = true;
				plugins.remove(state);
			}
		}

		return !bStop;
	}

	final private boolean stopThisPlugin(PluginState state) {
		boolean bDead = false;

		try {
			switch (state.getStatus()) {
			case PluginState.BUSY:
				if (state.getPluginAgent().isAlive() == false) {
					plugins.remove(state);
					PluginException pluginex = new PluginException(
							DBSWSException.SEVERITY_WARNING,
							PluginException.ANT_TRANCEIVERTIMEOUT4);
					pluginex.logMessage(state.getName());
				} else if (state.isTimedOut()) {
					state.setStatus(PluginState.SIGNALED);
					state.startTimer();
					PluginException pluginex = new PluginException(
							DBSWSException.SEVERITY_WARNING,
							PluginException.ANT_TRANCEIVERTIMEOUT1);
					pluginex.logMessage(state.getName());
				}
				break;

			case PluginState.SIGNALED:
				if (state.isTimedOut()) {
					state.setStatus(PluginState.INTERRUPTED);
					state.getPluginAgent().interrupt();
					state.startTimer();
					PluginException pluginex = new PluginException(
							DBSWSException.SEVERITY_WARNING,
							PluginException.ANT_TRANCEIVERTIMEOUT2);
					pluginex.logMessage(state.getName());
				}
				break;

			case PluginState.INTERRUPTED:
				if (state.isTimedOut()) {
					state.kill(true);
					plugins.remove(state);
					PluginException pluginex = new PluginException(
							DBSWSException.SEVERITY_WARNING,
							PluginException.ANT_TRANCEIVERTIMEOUT3);
					pluginex.logMessage(state.getName());
				}
				break;

			case PluginState.IDLE:
			case PluginState.FREE:
			case PluginState.KILL:
			default:
				bDead = true;
				break;
			}
		} catch (Exception e) {
		}
		return bDead;
	}

	public void run(PluginAgent plugin) {
		plugin.wakeup();
	}

	public void cleanupPlugin() {
		Iterator<PluginState> it = plugins.values().iterator();
		while (it.hasNext()) {
			PluginState state = it.next();
			state.getPluginAgent().cleanupPlugin();
		}
	}

	public void cleanupPlugin(PluginAgent plugin) {
		plugin.cleanupPlugin();
	}

	synchronized protected void notifyIdle(PluginAgent plugin) {
		PluginState state = plugin.getState();
		state.setStatus(PluginState.IDLE);
	}

	protected void pluginFinished(PluginAgent plugin) {
		PluginState state = plugin.getState();
		if (state.getStatus() != PluginState.FREE) {
			state.reset();
			state.setStatus(PluginState.FREE);
			plugins.remove(state);
		}
	}

	public PluginAgent getAvailablePlugin() {
		return getAvailablePlugin(null);
	}

	public synchronized PluginAgent getAvailablePlugin(String name) {
		
		PluginAgent plugin = null;
		PluginState state = null;
		try {
			if (name == null) {
				name = DB;
			}
			if (name != null) {
				synchronized (plugins) {
					state = plugins.get(name);
					if (state != null) {
						plugin = state.getPluginAgent();
					}
				}
			}
		} catch (Exception e) {
			state = null;
		}

		try {
			if (state == null) {
				state = addPlugin(name);
				plugin = state.getPluginAgent();
			}

			if (plugin == null) {
				PluginException pluginex = new PluginException(
						DBSWSException.SEVERITY_FATAL,
						PluginException.ANT_PLUGINERROR);
				pluginex
						.logMessage("Exception thrown in pluginFactory [getAvailablePlugin]\n\tNo available plugins\n\tPlugin Name : "
								+ name);
			} else {
				state.setStatus(PluginState.BUSY);
			}
		} catch (Exception e) {
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_PLUGINERROR);
			pluginex
					.logMessage("Exception thrown in pluginFactory [getAvailablePlugin]\n\tPlugin Name : "
							+ name
							+ "\n\terror = "
							+ e.toString()
							+ " "
							+ e.getMessage());
		}
		return plugin;
	}

	public void shutdown(boolean force) {
		running = false;
		Iterator<PluginState> it = plugins.values().iterator();
		for (PluginState state = it.next(); it.hasNext(); state = it.next()) {
			state.kill(force);
		}

		plugins = null;
		transceiver = null;
	}

	public Map<String, PluginState> getPluginAgents() {
		return plugins;
	}

	public ITransceiver getServer() {
		return transceiver;
	}
}
