#include "core.h"
#include "eventrouter.h"

#include <circle/synchronize.h>
#include <assert.h>

CCore::CCore (CEventRouter *pEventRouter, unsigned nCore, boolean bNeedsWakeUp)
: m_pEventRouter (pEventRouter),
  m_nCore (nCore)
{
	assert (m_pEventRouter != 0);
	m_pEventRouter->RegisterCore (m_nCore, &m_EventQueue, bNeedsWakeUp);
}

CCore::~CCore (void)
{
}

void CCore::Run (void)
{
	for (;;)
	{
		Event PendingEvent;

		// Keep IRQs masked between observing an empty queue and sleeping so
		// an enqueue IPI cannot be handled just before WaitForInterrupt().
		EnterCritical (IRQ_LEVEL);
		boolean bHaveEvent = m_EventQueue.Pop (PendingEvent);
		if (!bHaveEvent)
		{
			WaitForInterrupt ();
		}
		LeaveCritical ();

		if (bHaveEvent)
		{
			HandleEvent (PendingEvent);
		}
	}
}

boolean CCore::SendEventToCore (unsigned nCore, const Event &EventToSend)
{
	Event RoutedEvent = EventToSend;
	RoutedEvent.sourceCore = static_cast<u8> (m_nCore);

	return m_pEventRouter->QueueEvent (nCore, RoutedEvent);
}

boolean CCore::Subscribe (EventType Type)
{
	return m_pEventRouter->Subscribe (m_nCore, Type);
}

boolean CCore::Unsubscribe (EventType Type)
{
	return m_pEventRouter->Unsubscribe (m_nCore, Type);
}

u32 CCore::PublishEvent (const Event &EventToPublish)
{
	return m_pEventRouter->Publish (m_nCore, EventToPublish);
}

void CCore::ProcessEvents (void)
{
	Event PendingEvent;
	while (m_EventQueue.Pop (PendingEvent))
	{
		HandleEvent (PendingEvent);
	}
}
