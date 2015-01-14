#include "CacheGroup.h"
#include "DataItem.h"
#include "Item.h"
#include "DataArray.h"

long CacheGroup::GetState( unsigned long * pUpdateRate, 
                     bool * pActive, 
                     char * * ppName, 
                     long * pTimeBias, 
                     float * pPercentDeadband, 
                     unsigned long * pLCID, 
                     unsigned long * phClientGroup, 
                     unsigned long * phServerGroup )
{
	*pUpdateRate      = getRevisedRate();
	*pActive          = getActive();
	*ppName = new char[m_strName.size()];
	m_strName.copy( *ppName, m_strName.length() );
	*pTimeBias        = getTimeBias();
	*pPercentDeadband = getDeadband();
	*pLCID            = getLCID();
	*phClientGroup    = getClientGroup();
	*phServerGroup    = getServerGroup();

	return RET_OK;
}

long CacheGroup::SetState(unsigned long * pRequestedUpdateRate, 
							unsigned long * pRevisedUpdateRate,
							bool * pActive, 
							long * pTimeBias, 
							float * pPercentDeadband, 
							unsigned long * pLCID, 
							unsigned long * phClientGroup)
{
	long hr = RET_OK;

	try
	{
		if(pRequestedUpdateRate) 
		{
			unsigned long temp = *pRequestedUpdateRate;
			unsigned long MinRate = 250;
			temp += (MinRate/2);
			temp /= MinRate;
			temp *= MinRate;
			setRevisedRate( temp );
		}

		if(pActive != 0)
		{
			setActive(*pActive);
		}

		if(phClientGroup != 0) 
		{
			setClientGroup(*phClientGroup);
		}

		if(pPercentDeadband != 0) 
		{
			setDeadband(*pPercentDeadband);
		}

		if(pLCID != 0) 
		{
			setLCID(*pLCID);
		}

		if(pTimeBias != 0) 
		{
			setTimeBias(*pTimeBias);
		}

		DATAITEM * diAnswer = new DATAITEM;
		DataItemInit(diAnswer);

		// Get the RevisedRate from the answer. Format is a double in seconds.
		double dRevisedRate = diAnswer->dblVal;
		setRevisedRate((unsigned long)(dRevisedRate * 1000.) );

		*pRevisedUpdateRate = getRevisedRate();
	}
	catch( long e )
	{
		//"Exception caught in CacheGroup::SetState() hr = %x\n"
		hr = e;
	}

	return hr;
}

long CacheGroup::SetName( char * szName )
{
	m_strName = szName;
	return RET_OK;
}


long CacheGroup::CloneGroup(char * szName, void ** ppCacheGroupClone)
{
	/*
	unsigned long hServerGroup;
	unsigned long RevisedUpdateRate;
	CacheGroup * ppCacheGroupClone;

	AddGroup( m_strName.data(), 
               m_bActive, 
               m_dwRequestedUpdateRate, 
               m_hClientGroup, 
               m_pTimeBias, 
               m_pPercentDeadband, 
               m_dwLCID, 
               &hServerGroup, 
               &RevisedUpdateRate, 
               IID_IUnknown, 
               &ppCacheGroupClone );
	*/
	return RET_NOTIMPL;
}
