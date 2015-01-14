package com.db.sws.mimehandlers;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.lang.reflect.Method;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.messaging.ResponseWriter;
import com.db.sws.pluginagent.PluginAgent;
import com.db.sws.pluginagent.PluginFactory;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.xmltools.xmlutils.IXMLInputSerializer;
import com.db.sws.xmltools.xmlutils.IXMLOutputSerializer;
import com.db.sws.xmltools.xmlutils.XMLSerializerFactory;

public class MimeXML_Soap extends MimeXML {

	static private int HANDLER_POS=1;
	private PluginAgent plugin = null;

	public MimeXML_Soap() {
	}

	public ByteArrayOutputStream process(ITransceiver transceiver, String xml) {

		ByteArrayOutputStream out = new ByteArrayOutputStream();

		try {
			if (bVerbose == true)
				System.out.println("=============== Incoming XML Transaction ===============\n"
								+ xml);

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

						IXMLInputSerializer inserial = XMLSerializerFactory
								.getInputSerializer();
						inserial.setPackage(plugin.getPackageName());
						Object obj = inserial.get(xml);
						if (obj != null) {
							Object ret = plugin.execute(methodName, obj);
					        IXMLOutputSerializer outSerializer = XMLSerializerFactory.getOutputSerializer();        
					        String resposneMsg=outSerializer.get(ret, true);
							new ResponseWriter(out).writeResponse(resposneMsg);
						}
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