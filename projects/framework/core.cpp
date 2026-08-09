#include "core.h"

#include <circle/multicore.h>
#include <circle/synchronize.h>

CCore::CCore (unsigned nCore)
: m_nCore (nCore)
{
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

boolean CCore::QueueEvent (const Event &EventToQueue)
{
	if (!m_EventQueue.Push (EventToQueue))
	{
		return FALSE;
	}

	if (m_nCore != 0)
	{
		CMultiCoreSupport::SendIPI (m_nCore, IPI_USER);
	}

	return TRUE;
}

void CCore::ProcessEvents (void)
{
	Event PendingEvent;
	while (m_EventQueue.Pop (PendingEvent))
	{
		HandleEvent (PendingEvent);
	}
}
