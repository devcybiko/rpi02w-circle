#include "dispatcher.h"

#include <circle/logger.h>
#include <circle/sched/scheduler.h>
#include <circle/timer.h>
#include <assert.h>
#include <string.h>

CDispatcher::CDispatcher (CEventQueue *pEventQueue, CScheduler *pScheduler)
: m_pEventQueue (pEventQueue),
  m_pScheduler (pScheduler),
  m_nServiceCount (0),
  m_bInitialized (FALSE)
{
	assert (m_pEventQueue != 0);

	for (unsigned nService = 0; nService < MaxServices; nService++)
	{
		m_pServices[nService] = 0;
	}
}

CDispatcher::~CDispatcher (void)
{
	RemoveServices ();
}

void CDispatcher::SetScheduler (CScheduler *pScheduler)
{
	assert (!m_bInitialized);
	m_pScheduler = pScheduler;
}

boolean CDispatcher::AddService (CService *pService)
{
	if (m_bInitialized || pService == 0 || m_nServiceCount == MaxServices)
	{
		delete pService;
		return FALSE;
	}

	for (unsigned nService = 0; nService < m_nServiceCount; nService++)
	{
		if (m_pServices[nService] == pService)
		{
			return FALSE;
		}
	}

	m_pServices[m_nServiceCount++] = pService;
	return TRUE;
}

void CDispatcher::RemoveServices (void)
{
	while (m_nServiceCount > 0)
	{
		delete m_pServices[--m_nServiceCount];
		m_pServices[m_nServiceCount] = 0;
	}

	m_bInitialized = FALSE;
}

boolean CDispatcher::InitServices (void)
{
	if (m_bInitialized)
	{
		return FALSE;
	}

	for (unsigned nService = 0; nService < m_nServiceCount; nService++)
	{
		CService *pService = m_pServices[nService];
		if (!pService->InitService ())
		{
			return FALSE;
		}

		pService->ResetTimer ();
	}

	m_bInitialized = TRUE;
	return TRUE;
}

CService *CDispatcher::FindService (const char *pName)
{
	if (pName == 0)
	{
		return 0;
	}

	for (unsigned nService = 0; nService < m_nServiceCount; nService++)
	{
		CService *pService = m_pServices[nService];
		if (strcmp (pService->m_name, pName) == 0)
		{
			return pService;
		}
	}

	return 0;
}
void CDispatcher::UpdateServices (void)
{
	assert (m_bInitialized);

	const u64 nNow = CTimer::GetClockTicks64 ();

	for (unsigned nService = 0; nService < m_nServiceCount; nService++)
	{
		CService *pService = m_pServices[nService];
		if (pService->IsUpdateDue (nNow))
		{
			pService->Update ();
			pService->ResetTimer ();
		}
	}

	if (m_pScheduler != 0)
	{
		m_pScheduler->Yield ();
	}
}

void CDispatcher::Dispatch (const Event &PendingEvent)
{
	for (unsigned nService = 0; nService < m_nServiceCount; nService++)
	{
		m_pServices[nService]->OnEvent (PendingEvent);
	}
}
