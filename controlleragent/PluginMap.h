package com.db.sws.pluginagent;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Hashtable;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.dbswsex.PluginException;
import com.db.sws.loader.ClasspathLoader;
import com.db.sws.messaging.Envelope;
import com.db.sws.messaging.Message;
import com.db.sws.transceiver.ITransceiver;


public class PluginMap {
	
	protected Plugin pluginObj;

	private Hashtable<String, Method> methods = new Hashtable<String, Method>();

	public PluginMap() {
	}

	@SuppressWarnings("unchecked")
	public void createInstance(String strName, ClasspathLoader loader)
			throws DBSWSException {

		Class<Plugin> pluginClass = null;
		try {

			if (loader == null)
				pluginClass = (Class<Plugin>)Class.forName(strName);
			else {
				pluginClass = (Class<Plugin>)loader.loadClass(strName);
			}
			if (pluginClass == null) {
				DBSWSException pluginex = new DBSWSException(
						DBSWSException.SEVERITY_FATAL,
						DBSWSException.GEN_CLASSNOTFOUND);
				pluginex.logMessage("Class not found error class name is "
						+ strName);
				throw pluginex;
			}

			pluginObj = (Plugin) pluginClass.newInstance();
			if (pluginObj == null) {
				DBSWSException pluginex = new DBSWSException(
						DBSWSException.SEVERITY_FATAL,
						DBSWSException.GEN_CLASSNOTFOUND);
				pluginex.logMessage("Unable to create instance of class "
						+ strName);
				throw pluginex;
			}

			pluginObj.setTransceiver(ITransceiver.getTransceiver());
			Method[] methodarray = pluginClass.getMethods();
			int length = methodarray.length;
			for (int i = 0; i < length; i++) {
				methods.put(methodarray[i].getName(), methodarray[i]);
			}
		} catch (DBSWSException e) {
			throw e;
		} catch (Exception e) {
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_CREATEINSTANCEFAILURE);
			pluginex.logMessage("Class not found error class name is "
					+ strName + " reason " + e.getMessage());
			throw pluginex;
		}
	}

	public void setCurrentMessage(Message msg) {
		pluginObj.setCurrentMessage(msg);
	}

	public boolean preProcessMessage(Envelope env) {
		return pluginObj.preProcessMessage(env);
	}

	public void setAgent(PluginAgent agent) {
		pluginObj.setAgent(agent);
	}

	public Object executeMethod(String name, boolean bLogError, Object ... args)
			throws DBSWSException {

		Object ret = null;

		try {
			if (name != null && name.length() > 0) {
				Method m = methods.get(name);
				if (m == null) {
					PluginException pluginex = new PluginException(
							DBSWSException.SEVERITY_FATAL,
							PluginException.ANT_METHODNOTSUPPORTED);
					if (bLogError == true) {
						pluginex.logMessage("Illegal method name value is "
								+ name);
					}
					throw pluginex;
				}
				ret = m.invoke(pluginObj, args);
			}
		} catch (DBSWSException e) {
			throw e;
		} catch (InvocationTargetException e) {
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL, 999);
			if (bLogError == true) {
				pluginex.logMessage("Error executing method " + name
						+ " InvocationTargetException thrown: " + e);
			}
			Throwable ex = e.getTargetException();
			if (bLogError == true) {
				pluginex.logMessage("Error executing method " + name
						+ " Target exception thrown: " + ex.getMessage());
			}
			ex.printStackTrace();
			throw pluginex;
		} catch (Exception e) {
			PluginException pluginex = new PluginException(
					DBSWSException.SEVERITY_FATAL, 999);
			if (bLogError == true) {
				pluginex.logMessage("Error executing method " + name
						+ " exception thrown: " + e);
			}
			throw pluginex;
		}
		return ret;
	}

	public void peerUpdate() {
		pluginObj.peerUpdate();
	}
}
