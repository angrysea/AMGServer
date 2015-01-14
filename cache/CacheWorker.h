#pragma once

#include <string>

#include "../controlleragent/Worker.h"

class CacheWorker : Worker 
{
public:
	CacheWorker(const Address & address) :
		Worker{ address }
	{		
	}

	void put(const std::string & name, const std::string & key, const std::string & value) 
	{
		Message message = null;

		try {
			message = new Message(address.getURL() + PUT);
			message.getReplyTo().setPlugin(PluginFactory.CACHE);
			bresponded = false;
			starttime = System.currentTimeMillis();
			Messenger.postMessage(message, name, key, value);
		} catch (Exception e) {
			starttime = 0;
			endtime = 0;
		}
	}

	void remove(std::string  name, std::string  key) {
		Message message = null;

		try {
			message = new Message(address.getURL() + REMOVE);
			message.getReplyTo().setPlugin(PluginFactory.CACHE);
			bresponded = false;
			starttime = System.currentTimeMillis();
			Messenger.postMessage(message, name, key);
		} catch (Exception e) {
			starttime = 0;
			endtime = 0;
		}
	}

	void setLocked(std::string  name, boolean b) {
		Message message = null;

		try {
			message = new Message(address.getURL() + SETLOCKED);
			message.getReplyTo().setPlugin(PluginFactory.CACHE);
			bresponded = false;
			starttime = System.currentTimeMillis();
			Messenger.postMessage(message, name, new Boolean(b));
		} catch (Exception e) {
			starttime = 0;
			endtime = 0;
		}
	}

	void doCacheCheck() {
		Message message = null;

		try {
			for (Entry<std::string , Cache> entry : CachePlugin.getNamedCaches().entrySet()) {
				message = new Message(address.getURL()
						+ "/CachePlugin/cacheCheck");
				message.getReplyTo().setPlugin("CachePlugin");
				bresponded = false;
				starttime = System.currentTimeMillis();
				Messenger.postMessage(message, entry.getKey());
			}
		} catch (Exception e) {
			starttime = 0;
			endtime = 0;
		}
	}

private:
	static const std::std::string  PUT = "/cache/put";
	static const std::std::string  REMOVE = "/cache/remove";
	static const std::std::string  SETLOCKED = "/cache/setLocked";
};
