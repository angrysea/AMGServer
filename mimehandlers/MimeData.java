package com.db.sws.mimehandlers;

import java.io.ByteArrayOutputStream;

import com.db.sws.http.Request;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiver.NetworkAgent;


public abstract class MimeData implements MimeBase {

	static NetworkAgent networkAgent = null;

	protected boolean bRollBackOnly = false;
	protected int contentLength = 0;
	protected final static short WAIT = 0;
	protected final static short CHECK = 1;
	protected final static short ROLLBACK = 2;
	protected final static short COMMIT = 3;
	protected final static short RETURN = 4;
	protected final static short COMPLETE = 5;
	protected boolean bVerbose = false;

	public MimeData() {
		bVerbose = ITransceiver.getTransceiver().getVerboseFlag();
	}

	public abstract boolean process(String trxdata);

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
		process(request.getRequest());
		return null;
	}

	public void init(String u, ITransceiver transceiver) {
	}

	public Object getObject() {
		return null;
	}

	static {
		networkAgent = (NetworkAgent) ITransceiver.getTransceiver().getService(
				"networkAgent");
	}

}
