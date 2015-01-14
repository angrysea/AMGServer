package com.db.sws.mimehandlers;

import java.io.ByteArrayOutputStream;

import com.db.sws.http.Request;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiver.NetworkAgent;


public abstract class MimeXML implements MimeBase {

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

	MimeXML() {
		bVerbose = ITransceiver.getTransceiver().getVerboseFlag();
	}

	public abstract ByteArrayOutputStream process(ITransceiver transceiver,
			String xml);

	public int getStatus() {
		return 200;
	}

	public String getContentType() {
		return null;
	}

	public int getContentLength() {
		return contentLength;
	}

	public ByteArrayOutputStream process(ITransceiver transceiver,
			Request request) {
		String xml = request.getRequest();
		return process(transceiver, xml);
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

	static {
		networkAgent = (NetworkAgent) ITransceiver.getTransceiver().getService(
				"networkAgent");
	}
}
