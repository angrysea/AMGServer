package com.db.sws.socket;

import java.io.DataOutputStream;
import java.io.InputStream;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiverutils.CachedThread;



public class CallbackSocket extends ClientSocket {

	private CallbackSocketListener listener = null;

	public CallbackSocket(CallbackSocketListener listener, String type,
			ITransceiver transceiver, BaseSocketServer baseSocketServer,
			CachedThread thread) {
		super(type, transceiver, baseSocketServer, thread);
		this.listener = listener;
	}

	synchronized protected boolean startConnection(InputStream in,
			DataOutputStream out) {

		input = in;
		output = out;

		try {
			StringBuffer b = new StringBuffer();
			while (in.available() > 0) {
				b.append(in.read());
			}

			listener.OnReceive(b.toString());
			input.close();
			input = null;

			output.flush();
			output.close();
		} catch (Exception e) {
			DBSWSException exMessage = new DBSWSException(
					DBSWSException.GEN_MESSAGE,
					DBSWSException.SEVERITY_SUCCESS);
			exMessage.logMessage("Start connection failed");
		}
		return interrupted;
	}
}
