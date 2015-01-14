#pragma once

#include "CachedThread.h"

class ThreadCache 
{
public:

	ThreadCache(const std::string & name, int cachesize, int priority, int idleTimeout) 
	{
		initialize();
	}

	ThreadCache(const ThreadCache & aCopy) :
		name{ aCopy.name }, cachesize{ aCopy.cachesize }, priority{ aCopy.priority }, idleTimeout{ aCopy.idleTimeout }
	{
		initialize();
	}
	void initialize()
	{
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		for (int i = 0; i < idlethreads; i++) {
			CachedThread t = createThread();
			t.next = freelist;
			freelist = t;
			t.start();
		}
	}

	int getIdleTimeout() const 
	{
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		return (threadcount <= idlethreads) ? -1 : idletimeout;
	}

	bool isFree(const CachedThread & t, bool timedout) 
	{
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		if (timedout && (threadcount > idlethreads)) {
			if (t.isTerminated() == false) {
				threadcount--;
				usedthreads--;
				notifyAll();
			}
			return false;
		} else if (cachesize == 0 || threadcount <= cachesize) {
			t.next = freelist;
			freelist = t;
			usedthreads--;
			notifyAll();
			return true;
		} else {
			if (t.isTerminated() == false) {
				threadcount--;
				usedthreads--;
				notifyAll();
			}
			return false;
		}
	}

	void isDead(const CachedThread & t) 
	{
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		if (t.isTerminated() == false) {
			threadcount--;
			notifyAll();
		}
	}

	CachedThread createThread() {
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		threadcount++;
		threadid++;
		return new CachedThread(this, threadid);
	}

	CachedThread allocateThread(bool waitp) {
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		CachedThread t = null;

		if (freelist != null) {
			t = freelist;
			freelist = freelist.next;
		} else {
			t = createThread();
		}

		return t;
	}

	void setCachesize(int newValue) 
	{
		cachesize = newValue;
		idlethreads = (cachesize >> 1);
	}

	void setIdleTimeout(int newValue) 
	{
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		this.idletimeout = newValue;
	}

	void setThreadPriority(int newValue) 
	{
		threadpriority = newValue;
	}

	int getThreadPriority() 
	{
		return threadpriority;
	}

	bool getThread(Runnable runnable, bool waitp) 
	{
		while (true) {
			CachedThread t = allocateThread(waitp);
			if (t != null) {
				usedthreads++;
				if (t.wakeup(runnable))
					return true;
			} else {
				return false;
			}
		}
	}

	CachedThread getThread(bool waitp) {
		while (true) {
			CachedThread t = allocateThread(waitp);
			if (t != null) {
				usedthreads++;
				return t;
			}
		}
	}

	void waitForCompletion() {
		std::unique_lock<std::mutex> ul(mx, std::try_to_lock_t());
		while (usedthreads > 0) {
			try {
				wait();
			} catch (InterruptedException ex) {
			}
		}
	}

private:
	static bool debug = false;

	static int DEFAULT_CACHESIZE = 5;

	int cachesize = DEFAULT_CACHESIZE;

	int threadcount = 0;

	int threadid = 0;

	int idlethreads = 0;

	int usedthreads = 0;

	CachedThread freelist = null;

	int idletimeout = 0;

	int threadpriority = 0;

	std::mutex mx;

};
