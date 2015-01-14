package com.db.sws.peer;

import java.util.Stack;

import com.db.sws.xmltools.parser.Attributes;
import com.db.sws.xmltools.parser.DefaultHandler;


final public class PeerParser extends DefaultHandler {

	private static final int NONE = 0;

	private static final int PEERENTRY = 1;

	private static final int IDENTITY = 2;

	private static final int NAME = 3;

	private static final int EMAIL = 4;

	private static final int TYPE = 5;

	private static final int TIME = 6;

	private PeerEntry peerEntry = null;

	private PeerNode peer = null;

	private PeerNode current = null;

	private PeerRoot root = null;

	private Stack<PeerNode> nodeStack = new Stack<PeerNode>();

	private int state = 0;

	public PeerParser(PeerNode root) {
		super();
		current = root;
	}

	public PeerParser(PeerRoot root) {
		super();
		this.root = root;
		current = root;
	}

	public void startElement(String uri, String tag, String qtag,
			Attributes attrs) {

		if (tag.equals("Peer")) {
			if (peer != null) {
				nodeStack.push(current);
				current = peer;
			}

			if (peerEntry != null) {
				peer = current.insert(peerEntry);
			}

			peerEntry = new PeerEntry();
			state = PEERENTRY;
		} else if (tag.equals("URI")) {
			state = IDENTITY;
		} else if (tag.equals("Name")) {
			state = NAME;
		} else if (tag.equals("Email")) {
			state = EMAIL;
		} else if (tag.equals("Type")) {
			state = TYPE;
		} else if (tag.equals("Time")) {
			state = TIME;
		}
	}

	public void characters(char buffer[], int start, int length) {

		switch (state) {
		case IDENTITY:
			peerEntry.setURL(new String(buffer, start, length));
			break;

		case NAME:
			peerEntry.setName(new String(buffer, start, length));
			break;

		case EMAIL:
			peerEntry.setEmail(new String(buffer, start, length));
			break;

		case TYPE:
			peerEntry.setType(new String(buffer, start, length));
			break;

		case TIME:
			peerEntry.setTime(new String(buffer, start, length));
			break;

		default:
			break;
		}
		state = NONE;
	}

	public void endElement(String uri, String name, String qname) {

		if (name.equals("Peer")) {
			if (peerEntry == null) {
				if (nodeStack.empty() == false) {
					current = nodeStack.peek();
					nodeStack.pop();
				}
			} else {
				if (peer != null) {
					nodeStack.push(current);
					current = peer;
				}

				try {
					if (root.isConnected(peerEntry) == null) {
						current.insert(peerEntry);
					}
				} catch (Exception e) {
					// not sure why this would happen but best just to ignore.
				}
			}
			peerEntry = null;
		}
	}

}