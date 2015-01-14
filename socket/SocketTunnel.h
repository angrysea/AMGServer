#pragma once

//#include <iostream>
#include "Socket.h"
#include "BufferedReader.h"
#include "..\..\threads\threads\runnable.h"
#include "..\..\threads\threads\thread.h"

class SocketTunnel : public Runnable
{
public:
	static const int max=102400;
	static const int eof=BufferedReader::eof;

	SocketTunnel(Socket * reader, Socket * writer, bool ed = false, DWORD timeout=180000)
		:reader(reader),
		 writer(writer),
		 thread(0),
		 eofdisconnect(ed),
		 idleTimeout(timeout) 
	{
	}

	virtual ~SocketTunnel() 
	{
	}

	void start() 
	{
		thread = new Thread(this);
		thread->start();
	}

	void stop() 
	{
		DWORD idleTimeOld = idleTimeout;
		idleTimeout=0;
		thread->interrupt();
		idleTimeout = idleTimeOld;
	}

	void join() 
	{
		thread->join();
	}

	void run() 
	{

		int read=0;
		DWORD lastReadTime = ::GetCurrentTime();

		do 
		{
			read=max;
			::Sleep(10);
			// See if there is any data.
			if(reader->receive(buffer, &read)==false) 
			{
				// If the other side has disconnected no sense in waiting until a
				// timeout occurs
				if(reader->getLastError()==WSA_OPERATION_ABORTED) 
					break;
				//Check to see how long it has been since our last successfull read
				DWORD timeSinceRead = ::GetCurrentTime() - lastReadTime;
				if(timeSinceRead > idleTimeout - 1000)
					break;
				continue;
			}

			if(read>0) 
			{
				if(writer->send(buffer, &read)==false) 
				{
					break;
				}
				lastReadTime = ::GetCurrentTime();
			}
			else if(eofdisconnect)
			{
				break;
			}

		} while(true);

		//std::cout << "Close Tunnel" << std::endl;
		reader->shutDown(SD_BOTH);
		writer->shutDown(SD_BOTH);
		reader->close();
		writer->close();
	}

	void setTimeOut(DWORD newValue) 
	{
		idleTimeout = newValue;
	}

private:
	char buffer[max];
	bool eofdisconnect;
	Socket * reader;
	Socket * writer;
	DWORD idleTimeout;
	Thread * thread;
};
