package com.db.sws.socket;

import java.io.File;
import java.io.FileInputStream;
import java.net.ServerSocket;
import java.security.KeyStore;
import java.security.Security;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLServerSocketFactory;

import com.db.sws.transceiver.ITransceiver;
import com.db.sws.transceiver.TransceiverConfig;



public class SecureSocketServer extends HttpSocketServer {

	protected ServerSocket createServerSocket() {

		try {
			Security.addProvider(new com.sun.net.ssl.internal.ssl.Provider());

			SSLContext ctx = SSLContext.getInstance("TLS");
			KeyManagerFactory kmf = KeyManagerFactory.getInstance("SunX509");
			KeyStore ks = KeyStore.getInstance("JKS");

			String keyFile = ITransceiver.getTransceiver().getConfigFromFile()
					.getProperty(TransceiverConfig.KEY_STORE,
							"XMLAgentKeyStore");
			if (keyFile == null)
				throw new Exception(
						"KeyStore for SSL is not in the configuration of XML-Agent.");

			File fileKeyStore = new File(keyFile);
			if (!fileKeyStore.exists())
				throw new Exception("KeyStore "
						+ fileKeyStore.getAbsolutePath() + " does not exist.");

			char[] passphrase = (ITransceiver.getTransceiver()
					.getConfigFromFile().getProperty(
					TransceiverConfig.KEY_STORE_PASSPHRASE, "seamaster"))
					.toCharArray();
			ks.load(new FileInputStream(fileKeyStore), passphrase);
			kmf.init(ks, passphrase);

			System.out.print("Starting SSL Listener...");
			ctx.init(kmf.getKeyManagers(), null, null);
			System.out.println("done.");

			SSLServerSocketFactory ssf = ctx.getServerSocketFactory();
			ssf.createServerSocket(port, Math.max(128, sockets));
		} catch (Exception e) {
		}
		return null;
	}
}
