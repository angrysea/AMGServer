package com.db.sws.pluginagent;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.dbswsex.PluginException;
import com.db.sws.messaging.Envelope;
import com.db.sws.registry.PluginEntry;
import com.db.sws.registry.PluginFile;
import com.db.sws.transceiver.ITransceiver;

class ServicePlugin extends PluginBase {

	protected PluginMap pluginMap = null;

	final public Object process(Envelope env) throws Exception {
		
		Object ret = null;
		try {
			if (pluginMap == null) {
				startPlugin(((PluginFile) ITransceiver.getTransceiver()
						.getService("pluginfile")).findEntry(env.getHeader()
						.getMessage().getAddress().getPlugin()));
			}
			pluginMap.setCurrentMessage(env.getHeader().getMessage());
			Object args[] = env.getBody().getcontentArray();
			String method = env.getHeader().getMessage().getAddress()
					.getMethod();
			ret = pluginMap.executeMethod(method, true, args);
		} catch (Exception e) {
			PluginException agentex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS, e.getMessage());
			throw agentex;
		}
		return ret;
	}

	public Object execute(String methodName, Object request) throws Exception {
		Object ret = null;
		try {
			if (pluginMap == null) {
				throw new Exception("Unknow error no Map");
			}
			ret = pluginMap.executeMethod(methodName, true, request);
		} catch (Exception e) {
			PluginException agentex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS, e.getMessage());
			throw agentex;
		}
		return ret;
	}	

	public final boolean preProcessMessage(Envelope env) {
		boolean bRet = false;
		try {
			if (pluginMap == null)
				bRet = true;
			else
				bRet = pluginMap.preProcessMessage(env);
		} catch (Exception ex) {
		}
		return bRet;
	}

	final public String process(String xml) throws Exception {
		PluginException agentex = new PluginException(
				DBSWSException.SEVERITY_FATAL,
				PluginException.ANT_OBJDOTRANS);
		agentex.logMessage(agentex);
		throw agentex;
	}

	public final void startPlugin(PluginEntry entry) throws Exception {
		try {
			if (pluginMap == null) {
				createPluginMap();
			}
			pluginMap.createInstance(entry.getType(), loader);
			pluginMap.executeMethod("init", true, (Object [])null);
		} catch (Exception e) {
			PluginException agentex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS);
			agentex
					.logMessage("End document Exception error performing do transaction. "
							+ e.getMessage());
			throw e;
		}
	}

	public final void cleanupPlugin() {
		try {
			if (pluginMap != null) {
				pluginMap.executeMethod("cleanup", true, (Object [])null);
			}
		} catch (Exception e) {
			PluginException agentex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS);
			agentex
					.logMessage("End document Exception error performing do transaction. "
							+ e.getMessage());
		}
	}

	void createPluginMap() {
		pluginMap = new PluginMap();
	}
}
