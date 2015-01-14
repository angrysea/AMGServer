package com.db.sws.mimehandlers;

import java.io.ByteArrayOutputStream;

import com.db.sws.http.Request;
import com.db.sws.transceiver.ITransceiver;


public interface MimeBase {
	public abstract ByteArrayOutputStream process(ITransceiver transceiver,
			Request request);

	public abstract int getStatus();

	public abstract void init(String u, ITransceiver s);

	public String getContentType();
}
