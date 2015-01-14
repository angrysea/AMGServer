package com.db.sws.pluginagent;

import com.db.sws.loader.ClasspathLoader;
import com.db.sws.messaging.Envelope;
import com.db.sws.registry.PluginEntry;

public abstract class PluginBase {
	
	protected String name = null;

	protected ClasspathLoader loader = null;

	@SuppressWarnings("unused")
	private PluginAgent agent = null;

	public PluginBase() {
	}

	public void init(ClasspathLoader loader, PluginAgent agent) {
		this.agent = agent;
		this.loader = loader;
	}

	public void startPlugin(PluginEntry entry) throws Exception {
		throw new Exception("MethodNotSupported");
	}

	public void cleanupPlugin() {
	}

	public abstract boolean preProcessMessage(Envelope env);

	public abstract Object process(Envelope env) throws Exception;

	public abstract Object execute(String methodName, Object request) throws Exception;
}
