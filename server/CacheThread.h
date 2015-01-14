#pragma once

#include <memory>
#include <thread>
#include <mutex>

#include "../peer/PeerRoot.h"

class CacheThread 
{
public:

	CacheThread(std::shared_ptr<PeerRoot> peerRoot) : root{ peerRoot }
	{
	}

	void start();
	void run();

	void join();
	void join(long wait);
	bool isAlive();
	void stop();

	void setConnected(bool newValue) 
	{
		bConnected = newValue;
	}

private:
	std::shared_ptr<PeerRoot> root;
	std::unique_ptr<std::thread> runner = nullptr;
	std::mutex mx;
	bool bConnected = false;
	bool bStopped = false;
};
