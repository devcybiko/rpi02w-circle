#include "eventrouter.h"

#include <circle/multicore.h>
#include <assert.h>

CEventRouter::CEventRouter (void)
{
	for (unsigned nCore = 0; nCore < CoreCount; nCore++)
	{
		m_Endpoints[nCore].pQueue = 0;
		m_Endpoints[nCore].bNeedsWakeUp = FALSE;
		m_Subscriptions[nCore] = 0;
	}
}

void CEventRouter::RegisterCore (unsigned nCore, CEventQueue *pQueue, boolean bNeedsWakeUp)
{
	assert (nCore < CoreCount);
	assert (pQueue != 0);

	m_Lock.Acquire ();
	assert (m_Endpoints[nCore].pQueue == 0);
	m_Endpoints[nCore].pQueue = pQueue;
	m_Endpoints[nCore].bNeedsWakeUp = bNeedsWakeUp;
	m_Lock.Release ();
}

boolean CEventRouter::QueueEvent (unsigned nCore, const Event &EventToQueue)
{
	if (nCore >= CoreCount)
	{
		return FALSE;
	}

	m_Lock.Acquire ();
	TCoreEndpoint Endpoint = m_Endpoints[nCore];
	m_Lock.Release ();

	if (Endpoint.pQueue == 0 || !Endpoint.pQueue->Push (EventToQueue))
	{
		return FALSE;
	}

	if (Endpoint.bNeedsWakeUp)
	{
		CMultiCoreSupport::SendIPI (nCore, IPI_USER);
	}

	return TRUE;
}

boolean CEventRouter::Subscribe (unsigned nCore, EventType Type)
{
	if (nCore >= CoreCount || Type == EventType::None || Type >= EventType::Count)
	{
		return FALSE;
	}

	m_Lock.Acquire ();
	if (m_Endpoints[nCore].pQueue == 0)
	{
		m_Lock.Release ();
		return FALSE;
	}
	m_Subscriptions[nCore] |= EventBit (Type);
	m_Lock.Release ();

	return TRUE;
}

boolean CEventRouter::Unsubscribe (unsigned nCore, EventType Type)
{
	if (nCore >= CoreCount || Type == EventType::None || Type >= EventType::Count)
	{
		return FALSE;
	}

	m_Lock.Acquire ();
	if (m_Endpoints[nCore].pQueue == 0)
	{
		m_Lock.Release ();
		return FALSE;
	}
	m_Subscriptions[nCore] &= ~EventBit (Type);
	m_Lock.Release ();

	return TRUE;
}

u32 CEventRouter::Publish (unsigned nSourceCore, const Event &EventToPublish)
{
	if (nSourceCore >= CoreCount || EventToPublish.type == EventType::None
	    || EventToPublish.type >= EventType::Count)
	{
		return 0;
	}

	boolean bRecipients[CoreCount] = {};
	const EventMask TypeBit = EventBit (EventToPublish.type);

	// Snapshot the subscribers so queue operations never occur while the
	// router lock is held.
	m_Lock.Acquire ();
	for (unsigned nCore = 0; nCore < CoreCount; nCore++)
	{
		if (nCore != nSourceCore && (m_Subscriptions[nCore] & TypeBit) != 0)
		{
			bRecipients[nCore] = m_Endpoints[nCore].pQueue != 0;
		}
	}
	m_Lock.Release ();

	Event RoutedEvent = EventToPublish;
	RoutedEvent.sourceCore = static_cast<u8> (nSourceCore);

	u32 nDeliveredMask = 0;
	for (unsigned nCore = 0; nCore < CoreCount; nCore++)
	{
		if (bRecipients[nCore] && QueueEvent (nCore, RoutedEvent))
		{
			nDeliveredMask |= 1U << nCore;
		}
	}

	return nDeliveredMask;
}
