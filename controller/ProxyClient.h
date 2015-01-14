package com.db.sws.plugins;

import java.util.LinkedList;

import com.db.sws.socket.BaseSocketServer;
import com.db.sws.socket.CallbackSocketListener;
import com.db.sws.socket.CallbackSocketServer;
import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiverutils.Semaphore;



public class ProxyClient implements CallbackSocketListener {

	LinkedList<Object> messages = new LinkedList<Object>();

	CallbackSocketServer server = null;

	Semaphore sem = new Semaphore();

	static int staticport = 7070;

	public ProxyClient() {
		server = (CallbackSocketServer) BaseSocketServer
				.createInstance("CALLBACK");
		server.setSocketListener(this);
	}

	public void OnReceive(Object obj) {
		try {
			synchronized (messages) {
				messages.add(obj);
			}
			sem.semPost();
		} catch (Exception e) {
		}
	}

	public Object removeFirst() {

		sem.semWait();

		synchronized (messages) {
			return messages.removeFirst();
		}
	}

	public Object requestProxy() {

		while (true) {
			try {
				requestProxy(staticport);
			} catch (Exception e) {
				staticport++;
				if (staticport > 8000)
					return new Integer(0);
				continue;
			}
			break;
		}
		Integer ret = new Integer(staticport);
		staticport++;
		return ret;
	}

	public void requestProxy(int port) throws Exception {

		server.initialize(ITransceiver.getTransceiver(), staticport, 30);
	}
}