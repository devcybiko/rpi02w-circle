#include "core.h"
#include "eventrouter.h"

#include <circle/logger.h>
#include <circle/multicore.h>
#include <circle/synchronize.h>
#include <assert.h>

CCore *CCore::s_pCores[CEventRouter::CoreCount] = {};
CCore *pCore0 = 0;
CCore *pCore1 = 0;
CCore *pCore2 = 0;
CCore *pCore3 = 0;

CCore::CCore (CEventRouter *pEventRouter, unsigned nCore, boolean bNeedsWakeUp)
: m_pEventRouter (pEventRouter),
  m_nCore (nCore),
  m_Dispatcher (&m_EventQueue)
{
	assert (m_pEventRouter != 0);
	assert (m_nCore < CEventRouter::CoreCount);
	assert (s_pCores[m_nCore] == 0);
	s_pCores[m_nCore] = this;
	if (m_nCore == 0)
	{
		pCore0 = this;
	}
	else if (m_nCore == 1)
	{
		pCore1 = this;
	}
	else if (m_nCore == 2)
	{
		pCore2 = this;
	}
	else if (m_nCore == 3)
	{
		pCore3 = this;
	}
	m_pEventRouter->RegisterCore (m_nCore, &m_EventQueue, bNeedsWakeUp);
}

CCore::~CCore (void)
{
	s_pCores[m_nCore] = 0;
}

CCore *CCore::GetCore (unsigned nCore)
{
	return nCore < CEventRouter::CoreCount ? s_pCores[nCore] : 0;
}

boolean CCore::Post (const Event &EventToPost)
{
	Event RoutedEvent = EventToPost;
	RoutedEvent.sourceCore = static_cast<u8> (CMultiCoreSupport::ThisCore ());

	return m_pEventRouter->QueueEvent (m_nCore, RoutedEvent);
}

u32 CCore::Publish (const Event &EventToPublish)
{
	return m_pEventRouter->Publish (CMultiCoreSupport::ThisCore (), EventToPublish);
}

void CCore::Run (void)
{
	m_Dispatcher.InitServices ();
	Publish (Event::Log (LogNotice, "Core %u initialized", m_nCore));
	for (;;)
	{

		m_Dispatcher.UpdateServices ();
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
			m_Dispatcher.Dispatch (PendingEvent);
		}
	}
}

boolean CCore::Subscribe (EventType Type)
{
	return m_pEventRouter->Subscribe (m_nCore, Type);
}

boolean CCore::Unsubscribe (EventType Type)
{
	return m_pEventRouter->Unsubscribe (m_nCore, Type);
}
