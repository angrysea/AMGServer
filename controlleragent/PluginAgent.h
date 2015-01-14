package com.db.sws.pluginagent;

import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.LinkedList;
import java.util.NoSuchElementException;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.dbswsex.PluginException;
import com.db.sws.messaging.Envelope;
import com.db.sws.messaging.Message;
import com.db.sws.messaging.Messenger;
import com.db.sws.registry.PluginEntry;
import com.db.sws.transceiverutils.CachedThread;
import com.db.sws.transceiverutils.Semaphore;


public final class PluginAgent implements Runnable {

	private LinkedList<Envelope> messages = new LinkedList<Envelope>();
	private Envelope env = null;
	@SuppressWarnings("unused")
	private static final boolean debug = true;
	private PluginState state = null;
	private Exception lastError = null;
	private PluginBase plugin = null;
	private CachedThread thread = null;
	private PluginFactory pluginFactory = null;
	private boolean verbose = false;
	private PluginEntry entry = null;
	private String name = null;
	private Semaphore semaphore = new Semaphore();

	public PluginAgent(boolean verbose) {
		this.verbose = verbose;
		this.state = new PluginState(this);
	}

	public PluginAgent(PluginFactory pluginFactory, CachedThread thread,
			boolean verbose) {
		this.thread = thread;
		this.verbose = verbose;
		this.pluginFactory = pluginFactory;
		this.state = new PluginState(this);
	}

	final public boolean wakeup() {
		return thread.wakeup(this);
	}

	public void run() {

		try {
			synchronized (this) {
				while ((env = peekMessage(true)) != null) {
					if (plugin.preProcessMessage(env) == true)
						process();
				}
			}
		} catch (DBSWSException ex) {
			ex.logMessage();
			lastError = ex;
		} catch (Exception e) {
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_ERRORDURINGMETHODEXECUTION);
			pluginex.logMessage(e.getMessage());
			lastError = e;
		} finally {
			semaphore.semPost();
		}

		if (verbose == true)
			System.out.println("=============== run complete ==================\n");
	}

	final public void join() {

		if (thread != null) {
			while (true) {
				try {
					thread.join();
				} catch (InterruptedException ex) {
					System.out.println(ex);
				}
			}
		}
	}

	public void reset(CachedThread t) {
		thread = t;
		reset();
	}

	public void reset() {
		entry = null;
		plugin = null;
	}

	public Object process() throws DBSWSException {

		Object ret = null;
		try {
			if (plugin == null) {
				PluginException pluginex = new PluginException(
						DBSWSException.SEVERITY_FATAL,
						PluginException.ANT_PARSER);
				pluginex.logMessage("Exception thrown plugin was not preparsed");
				throw pluginex;
			}

			ret = plugin.process(env);

			if (pluginFactory != null) {
				pluginFactory.notifyIdle(this);
			}

		} catch (DBSWSException e) {
			Message message = Message.createReply(env.getHeader().getMessage());
			message.setMethod("error");
			Object args[] = new Object[2];
			args[0] = new String(message.getAddress().getURI());
			args[1] = new String(e.getMessage());
			Messenger.postMessage(message, args);
		} catch (Exception e) {
			if (pluginFactory != null) {
				pluginFactory.pluginFinished(this);
			}
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_PARSER, e + ": " + e.getMessage());
			// pluginex.logMessage(e+": "+e.getMessage());
			throw pluginex;
		}
		return ret;
	}

	public Object process(Envelope envelope) throws DBSWSException {

		Object ret = null;
		try {
			if (plugin == null) {
				PluginException pluginex = new PluginException(
						DBSWSException.SEVERITY_FATAL,
						PluginException.ANT_PARSER);
				pluginex.logMessage("Exception thrown plugin was not preparsed");
				throw pluginex;
			}
			ret = plugin.process(envelope);
		} catch (Exception e) {
			if (pluginFactory != null) {
				pluginFactory.pluginFinished(this);
			}
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_PARSER, e + ": " + e.getMessage());
			// pluginex.logMessage(e+": "+e.getMessage());
			throw pluginex;
		}
		return ret;
	}

	public Object execute(String methodName, Object arg) throws DBSWSException {

		Object ret = null;
		try {
			if (plugin == null) {
				PluginException pluginex = new PluginException(
						DBSWSException.SEVERITY_FATAL,
						PluginException.ANT_PARSER);
				pluginex.logMessage("Exception thrown plugin was not preparsed");
				throw pluginex;
			}
			ret = plugin.execute(methodName, arg);
		} catch (Exception e) {
			if (pluginFactory != null) {
				pluginFactory.pluginFinished(this);
			}
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_PARSER, e + ": " + e.getMessage());
			// pluginex.logMessage(e+": "+e.getMessage());
			throw pluginex;
		}
		return ret;
	}

	public void preProcess(String className) throws DBSWSException {

		try {
			if (plugin == null) {
				plugin = (PluginBase) Class.forName(className).newInstance();
				plugin.init(state.getLoader(), this);
			}

			if (plugin == null) {
				PluginException pluginex = new PluginException(
						DBSWSException.SEVERITY_FATAL,
						PluginException.ANT_CLASSERROR);
				pluginex
						.logMessage("[preparse]Error loading Plugin unable to load class "
								+ className);
				throw pluginex;
			}
			if (state != null) {
				state.setTimeOut(new Integer("-1").intValue());
			}
		} catch (DBSWSException e) {

			if (pluginFactory != null) {
				pluginFactory.pluginFinished(this);
			}
			throw e;
		} catch (Exception e) {
			if (pluginFactory != null) {
				pluginFactory.pluginFinished(this);
			}

			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_CLASSERROR);
			pluginex.logMessage("Error loading Plugin unable to load class "
					+ className + " Exception thrown during message: " + e
					+ " " + e.getMessage());
			throw pluginex;
		}
	}

	public void peerUpdate() {
	}

	public void startPlugin(PluginEntry entry) throws Exception {
		if(!state.isStarted()) {
			plugin.startPlugin(entry);
			state.setStarted(true);
		}
	}

	public void startPlugin() throws Exception {
		if(!state.isStarted()) {
			plugin.startPlugin(this.entry);
			state.setStarted(true);
		}
	}

	public boolean isStarted() {
		return state.isStarted();

	}
	
	public final Exception getLastError() {
		Exception e = lastError;
		lastError = null;
		return e;
	}

	public final boolean hasError() {
		return lastError != null;
	}

	public final PluginState getState() {
		return state;
	}

	public String getName() {
		return name;
	}

	public void setName(String newValue) {
		name=newValue;
	}

	public String getClasspath() {
		return entry.getClasspath();
	}

	public String getPackageName() {
		return entry.getPackageName();
	}

	public PluginEntry getEntry() {
		return entry;
	}

	void setEntry(PluginEntry newValue) {
		entry = newValue;
	}

	public void setTransactionTimeout(int i) {
		state.setTimeOut(i);
	}

	public void cleanupPlugin() {
		try {
			plugin.cleanupPlugin();
		} catch (Exception e) {
		}
	}

	public void interrupt() {
		thread.notify();
	}

	public boolean kill() {
		return thread.kill();
	}

	public boolean isAlive() {
		return thread.isAlive();
	}

	public Semaphore getSemaphore() {
		return semaphore;
	}

	@SuppressWarnings("unused")
	private StringBuffer loadURL(String strUrl) {

		try {
			URL url = new URL(strUrl);
			URLConnection connection = url.openConnection();
			InputStream is = connection.getInputStream();
			StringBuffer stringbuffer = new StringBuffer();

			// read the response
			int b = -1;
			while (true) {
				b = is.read();
				if (b == -1)
					break;
				stringbuffer.append((char) b);
			}
			is.close();
			return stringbuffer;
		} catch (Exception e) {
			return null;
		}
	}

	public void setEnvelope(Envelope env) {
		this.env = env;
	}

	public void pushMessage(Envelope env) {
		messages.add(env);
	}

	public Envelope getMessage() {
		try {
			while (true) {
				if (messages.size() > 0)
					return messages.removeFirst();
				else
					try {
						Thread.sleep(500);
					} catch (InterruptedException e) {
					}
			}
		} catch (NoSuchElementException e) {
		}
		return null;
	}

	public Envelope peekMessage() {
		return peekMessage(false);
	}

	public Envelope peekMessage(boolean bRemove) {

		try {
			if (bRemove)
				return messages.removeFirst();
			else
				return messages.getFirst();
		} catch (NoSuchElementException e) {
		}
		return null;
	}
}
