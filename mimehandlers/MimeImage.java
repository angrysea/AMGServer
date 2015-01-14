package com.db.sws.mimehandlers;

import java.io.ByteArrayOutputStream;

import com.db.sws.http.Request;
import com.db.sws.transceiver.ITransceiver;


public class MimeImage implements MimeBase {
	@SuppressWarnings("unused")
	private String url = null;

	private String contentType = null;

	public MimeImage(String url) {
		this.url = url;
	}

	public void init(String u, ITransceiver s) {
	}

	public String getContentType() {
		return contentType;
	}

	public ByteArrayOutputStream process(ITransceiver transceiver,
			Request request) {
		// TODO: Implement this com.db.sws.http.MimeBase method
		return null;
	}

	public int getStatus() {
		return 200;
	}
}
