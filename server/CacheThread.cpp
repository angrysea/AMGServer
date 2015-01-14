#include <chrono>

#include "CacheThread.h"
#include "../cache/CacheWorker.h"

void CacheThread::start() 
{
	runner = std::make_unique<std::thread>(std::thread(this));
}

void CacheThread::join() 
{
	runner->join();
}

void CacheThread::join(long wait) 
{
	runner->join(wait);
}

bool CacheThread::isAlive() 
{
	return runner->joinable();
}


void CacheThread::run() 
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	{
		std::lock_guard<std::mutex> lg(mx);
		if (root->count() > 0 && bConnected) 
		{
			root->cache();
		}
	}

	while (!bStopped) 
	{
		if (bConnected) {
			CacheWorker worker;
			Iterator<CacheWorker> it = CachePlugin.iterator();
			while (it.hasNext()) {
				worker = it.next();
				try {
					worker.doCacheCheck();
				}
				catch (Exception exx) {
					exx.printStackTrace();
				}
			}
		}

		// Run the maintenance loop every minute
		{
			std::lock_guard<std::mutex> lg(mx);
			std::this_thread::sleep_for(std::chrono::milliseconds(60000));
		}
	}
}

