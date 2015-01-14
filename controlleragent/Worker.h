#pragma once

#include "../messaging/Address.h"

class Worker {
public:
	Worker(const Address & address) :
		address{ address }
	{
		endtime = 0;
		starttime = 0;
	}

	const Address & getAddress() const
	{
		return address;
	}

	bool getResponded() const 
	{
		return bresponded;
	}

	void setResponded(bool newValue) 
	{
		bresponded = newValue;
	}

	void setEndTime(long newValue) 
	{
		endtime = newValue;
	}
	
	long getEndTime() const 
	{
		if (endtime == 0) 
		{
			return 0;
		}
		return (starttime - endtime);
	}

	long getStartTime() const
	{
		return starttime;
	}

	int hashCode() const 
	{
		return address.hashCode();
	}

	bool operator=(const Worker & worker) const
	{
		
		return hashCode() == worker.hashCode();
	}

protected:
	Address address;
	long starttime = 0;
	long endtime = 0;
	bool bresponded = false;
};
