package com.db.sws.socket;

import java.io.BufferedInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;

import com.db.sws.dbswsex.DBSWSException;
import com.db.sws.http.Parser;
import com.db.sws.http.Request;
import com.db.sws.http.Response;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiverutils.CachedThread;
import com.db.sws.xmltools.parser.InputSource;

public class ClientSocket extends BaseSocket {

	protected Socket socket = null;

	protected OutputStream output = null;

	public ClientSocket(String type, ITransceiver transceiver,
			BaseSocketServer baseSocketServer, CachedThread thread) {

		super(type, transceiver, baseSocketServer, thread);
	}

	synchronized protected boolean startConnection(InputStream in,
			DataOutputStream out) {

		input = in;
		output = out;

		try {
			Parser parser = new Parser(new InputSource(in), type);
			Request request = parser.parse();
			request.setSocket(socket);
			Response response = request.processRequest(transceiver, output);
			if (response != null) {
				response.respond();
			}
			input.close();
			output.flush();
			output.close();
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
			if (bufout == null) {
				bufout = new SocketOutputBuffer(socket.getOutputStream(),
						bufsize);
			} else {
				bufout.reuse(socket.getOutputStream());
			}
			startConnection(new BufferedInputStream(socket.getInputStream()),
					new DataOutputStream(bufout));
		} catch (IOException e) {
		} finally {
			baseSocketServer.socketFinished(this);
		}
	}

	public InetAddress getInetAddress() {
		return (socket != null) ? socket.getInetAddress() : null;
	}

	protected void stopConnection() {
		if (socket != null) {
			try {
				socket.close();
			} catch (Exception e) {
			}
			socket = null;
		}
	}

	public synchronized void bind(Socket s) {

		socket = s;
		try {
			socket.setSoTimeout(timeout);
		} catch (SocketException ex) {
		}
		idle = false;
		baseSocketServer.run(this);
	}

	protected synchronized void terminate() {
		if (running == true) {
			try {
				if (output != null) {
					output.flush();
					output.close();
				}
			} catch (IOException ex) {
				System.out.println(ex);
			}

			try {
				if (input != null) {
					input.close();
				}
			} catch (IOException ex) {
				System.out.println(ex);
			}

			input = null;
			output = null;
			interrupted = false;
			running = false;
		}
	}

	public int sendContinue() throws IOException {

		if (cont == true) {
			return -1;
		}

		output.flush();
		cont = true;
		return 0;
	}

	public OutputStream getOutputStream() {
		return output;
	}
}
