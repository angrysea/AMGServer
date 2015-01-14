package com.db.sws.mimehandlers;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.lang.reflect.Method;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.http.Request;
import com.db.sws.messaging.ResponseWriter;
import com.db.sws.pluginagent.PluginAgent;
import com.db.sws.pluginagent.PluginFactory;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiver.NetworkAgent;

public class Mime_Binary implements MimeBase {

	static private int HANDLER_POS=1;
	private PluginAgent plugin = null;
	protected boolean bRollBackOnly = false;
	protected int contentLength = 0;
	protected final static short WAIT = 0;
	protected final static short CHECK = 1;
	protected final static short ROLLBACK = 2;
	protected final static short COMMIT = 3;
	protected final static short RETURN = 4;
	protected final static short COMPLETE = 5;
	protected boolean bVerbose = false;
	protected String url;
	
	static NetworkAgent networkAgent = null;

	static {
		networkAgent = (NetworkAgent) ITransceiver.getTransceiver().getService(
				"networkAgent");
	}
	

	public Mime_Binary() {
		bVerbose = ITransceiver.getTransceiver().getVerboseFlag();
	}
	
	public int getStatus() {
		return 200;
	}

	public String getContentType() {
		return null;
	}

	public int getContentLength() {
		return contentLength;
	}

	public void init(String url, ITransceiver transceiver) {
		this.url=url;
		
		synchronized(networkAgent) {
			if(networkAgent==null) {
				networkAgent = (NetworkAgent) transceiver.getService("networkAgent");			
			}
		}
	}

	public Object getObject() {
		return null;
	}

	public ByteArrayOutputStream process(ITransceiver transceiver, Request request) {

		ByteArrayOutputStream out = new ByteArrayOutputStream();

		try {
			if (bVerbose == true)
				System.out.println("=============== Incoming XML Transaction ===============\n"
								+ request.getRequestURI());

			String s[] = url.split("/");

			if (s != null && s.length > HANDLER_POS) {
				String name = s[HANDLER_POS];
				String methodName = s[HANDLER_POS + 1];

				if (name != null && name.length()>0) {
					plugin = (PluginAgent)transceiver.getAvailablePlugin(name);
					if(plugin!=null) {
						plugin.preProcess(PluginFactory.SERVICECLASS);
						plugin.startPlugin();

						DBSWSException exMessage = new DBSWSException(
								DBSWSException.SEVERITY_SUCCESS,
								DBSWSException.GEN_MESSAGE);
						exMessage.logMessage("Plugin received Name: "
								+ plugin.getName());

						Object ret = plugin.execute(methodName, request.getData());
						new ResponseWriter(out, false).writeResponse((byte [])ret);
					}
				}
			} else {
				DBSWSException exMessage = new DBSWSException(
						DBSWSException.SEVERITY_ERROR,
						DBSWSException.GEN_BASE);
				exMessage.logMessage("Unable to load find available handler: ");
			}

			DBSWSException exMessage = null;


			exMessage = new DBSWSException(
					DBSWSException.SEVERITY_SUCCESS,
					DBSWSException.GEN_MESSAGE);
			exMessage.logMessage("Plugin successfully executed Name: "
					/*+ plugin.getName()*/);
		} catch (Exception e) {
			DBSWSException exMessage = new DBSWSException(
					DBSWSException.GEN_MESSAGE,
					DBSWSException.SEVERITY_SUCCESS);
			exMessage.logMessage("Execution failed reason: " + e.getMessage());
			try {
				out.write(("<?xml version=\"1.0\" encoding=\"UTF-8\" ?><status><plugin>unknown</plugin><code>1</code><desc>"
								+ e.getMessage()
								+ "</desc><timestamp>"
								+ (new java.util.Date(System
										.currentTimeMillis()).toString()) + "</timestamp></status>")
								.getBytes());
			} catch (IOException ioe) {
			}
		} finally {
		}

		return out;
	}
	
	protected Object executeMethod(Object targetObject, String name,
			Object[] args) throws Exception {
		Object ret = null;
		for (Method method : targetObject.getClass().getDeclaredMethods()) {
			if (method.getName().equalsIgnoreCase(name)) {
				boolean bExecute = true;
				@SuppressWarnings("rawtypes")
				Class pvec[] = method.getParameterTypes();

				if (args == null) {
					if (pvec != null && pvec.length != 0) {
						bExecute = false;
					}
				} else if (pvec.length == args.length) {
					bExecute = true;
					for (int i = 0; i < pvec.length; i++) {
						if (pvec[i] != args[i].getClass()) {
							bExecute = false;
							break;
						}
					}
				}
				if (bExecute) {
					try {
						ret = method.invoke(targetObject, args);
					} catch (Exception e) {
						throw new Exception("Error executing method.");
					}
				} else {
					throw new Exception("No Such Method Found.");
				}
			}
		}
		return ret;
	}
}