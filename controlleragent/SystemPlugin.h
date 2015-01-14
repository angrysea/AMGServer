package com.db.sws.pluginagent;

public abstract class SystemPlugin extends PluginBase {

	public Object execute(String methodName, Object request) throws Exception {
		throw new Exception("Execute not implemented");
	}
}
