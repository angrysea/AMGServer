#pragma once
#include <mutex>
#include <chrono>
#include <thread>
#include <string>

class CachedThread 
{
public:

	CachedThread(ThreadCache threadCache, int id) 
	{
		cache = threadCache;
		setPriority(cache.getThreadPriority());
		setDaemon(true);
	}

	bool isTerminated() 
	{
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		bool ret = terminated;
		terminated = true;
		return ret;
	}

	Runnable waitForRunner() 
	{
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		bool bTimedout = false;

		while (alive == true) {
			if (runner != null) {
				Runnable timeOutRunner = runner;
				firstime = false;
				runner = null;
				return timeOutRunner;
			} else if (firstime) {
				try {
					wait();
				} catch (InterruptedException ex) {
				}
			} else if (alive = cache.isFree(this, bTimedout)) {
				try {
					int idleTimeout = cache.getIdleTimeout();
					if (idleTimeout > 0) {
						wait(idleTimeout);
					} else {
						wait();
					}
				} catch (InterruptedException ex) {
				}
			}
		}
		return null;
	}

	bool kill() {
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		if (bRunning == true) {
			alive = false;
			bRunning = false;
			this.notifyAll();
		}
		return bRunning;
	}

	bool wakeup(Runnable runnable) {
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		bool ret = false;
		if (alive) {
			runner = runnable;
			if (started == false) {
				this.start();
			}
			notify();
			ret = true;
		}
		return ret;
	}

	void start() {
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		super.start();
		this.started = true;
	}

	void run() {
		try {
			while (true) {
				Runnable timeOutRunner = waitForRunner();
				if (timeOutRunner != null) {
					bRunning = true;
					timeOutRunner.run();
					bRunning = false;
				}
				if (alive == false) {
					break;
				}
			}
		} catch (NoSuchMethodError e) {
		} finally {
			cache.isDead(this);
		}
	}
private:
	std::mutex mx;
	ThreadCache cache;
	CachedThread next;
	bool alive = true;
	bool terminated = false;
	bool started = false;
	bool firstime = true;
	bool bRunning = false;
};
