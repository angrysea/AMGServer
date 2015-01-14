package com.db.sws.socket;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.dbswsex.PluginException;
import com.db.sws.http.Parser;
import com.db.sws.http.Request;
import com.db.sws.messaging.Message;
import com.db.sws.messaging.PostWriter;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiverutils.CachedThread;
import com.db.sws.xmltools.parser.InputSource;

public class ProxySocket extends BaseSocket {

	private Message msg = null;

	private PostWriter writer = new PostWriter();

	private Object args[] = new Object[1];

	public ProxySocket(String type, ITransceiver transceiver,
			BaseSocketServer baseSocketServer, CachedThread thread) {

		super(type, transceiver, baseSocketServer, thread);
		address = "http://" + transceiver.getProxyAddress();
		msg = new Message(address);
		msg.getAddress().setMethod("getRequest");
		msg.getAddress().setPlugin("ProxyServer");
		msg.getAddress().setSync();

		args[0] = new String(transceiver.getHost());
	}

	synchronized private boolean startConnection(InputStream in) {

		try {
			Parser parser = new Parser(new InputSource(in), type);
			Request request = parser.parse();
			if (request.getRequestType() == Request.NONE) {
				return interrupted;
			}

			request.setSocket(null);
			request.processRequest(transceiver, null);
			input.close();
		} catch (DBSWSException e) {
			e.printStackTrace();
		} catch (Exception e) {
			DBSWSException exMessage = new DBSWSException(
					DBSWSException.GEN_MESSAGE,
					DBSWSException.SEVERITY_SUCCESS);
			exMessage.logMessage("Start connection failed");
		}
		return interrupted;
	}

	public void run() {
		try {
			startConnection(new BufferedInputStream(input));
		} catch (Exception e) {
		} finally {
			baseSocketServer.socketFinished(this);
		}
	}

	protected synchronized void terminate() {

		if (running == true) {
			try {
				if (input != null) {
					input.close();
				}
			} catch (IOException ex) {
				System.out.println(ex);
			}

			input = null;
			interrupted = false;
			running = false;
		}
	}

	protected synchronized void interruptConnection(boolean now) {

		if (running == true) {
			interrupted = true;
			if (now == true) {
				terminate();
			}
		}
	}

	public int sendContinue() throws IOException {

		if (cont == true) {
			return -1;
		}

		cont = true;
		return 0;
	}

	public OutputStream getOutputStream() {
		return null;
	}

	public void requestProxy() throws Exception {

		try {
			Message requestMsg = new Message(msg);
			requestMsg.setMethod("requestProxy");

			URL url = new URL(address);
			URLConnection connection = url.openConnection();

			connection.setDoOutput(true);
			connection.setRequestProperty("Content-Type", " " + "text/xml");
			connection.setRequestProperty("Connection", " close");
			OutputStream os = connection.getOutputStream();

			writer.setStream(os);
			writer.write(requestMsg, args);

			connection.connect();
			os.flush();
			os.close();

			input = connection.getInputStream();

			InputStreamReader reader = new InputStreamReader(
					new BufferedInputStream(connection.getInputStream()));
			StringBuffer stringbuffer = new StringBuffer();

			int b = -1;
			while ((b = reader.read()) != -1) {
				stringbuffer.append((char) b);
			}

			String result = stringbuffer.toString();
			reader.close();

			if (result.length() < 1)
				throw new Exception("Unknown return no results");
		} catch (Exception e) {
			throw new PluginException(DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS);
		}
	}

	public void bind() throws Exception {

		try {
			URL url = new URL(address);
			URLConnection connection = url.openConnection();

			connection.setDoOutput(true);
			connection.setRequestProperty("Content-Type", " " + "text/xml");
			connection.setRequestProperty("Connection", " close");
			OutputStream os = connection.getOutputStream();

			writer.setStream(os);
			writer.write(msg, args);

			connection.connect();
			os.flush();
			os.close();

			input = connection.getInputStream();
			try {
				while ((input.available()) < 1) {
					try {
						Thread.sleep(50);
					} catch (InterruptedException ex) {
					}
				}
				baseSocketServer.run(this);
			} catch (IOException ioe) {
				ioe.printStackTrace();
			}
		} catch (Exception e) {
			throw new PluginException(DBSWSException.SEVERITY_FATAL,
					PluginException.ANT_OBJDOTRANS);
		}
	}
}
