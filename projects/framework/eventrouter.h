#pragma once

#include "eventqueue.h"

#include <circle/spinlock.h>
#include <circle/types.h>

class CEventRouter
{
public:
	static constexpr unsigned CoreCount = 4;

	CEventRouter (void);

	void RegisterCore (unsigned nCore, CEventQueue *pQueue, boolean bNeedsWakeUp);
	boolean QueueEvent (unsigned nCore, const Event &EventToQueue);
	boolean Subscribe (unsigned nCore, EventType Type);
	boolean Unsubscribe (unsigned nCore, EventType Type);
	u32 Publish (unsigned nSourceCore, const Event &EventToPublish);

private:
	struct TCoreEndpoint
	{
		CEventQueue *pQueue;
		boolean bNeedsWakeUp;
	};

	TCoreEndpoint m_Endpoints[CoreCount];
	EventMask m_Subscriptions[CoreCount];
	CSpinLock m_Lock;
};
