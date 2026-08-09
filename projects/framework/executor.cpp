#include "executor.h"

#include <circle/timer.h>
#include <assert.h>

CExecutor::CExecutor (CEventQueue *pEventQueue)
: m_pEventQueue (pEventQueue),
  m_nServiceCount (0),
  m_bInitialized (FALSE)
{
	assert (m_pEventQueue != 0);

	for (unsigned nService = 0; nService < MaxServices; nService++)
	{
		m_pServices[nService] = 0;
	}
}

boolean CExecutor::RegisterService (CService *pService)
{
	if (m_bInitialized || pService == 0 || m_nServiceCount == MaxServices)
	{
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

boolean CExecutor::Initialize (void)
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

void CExecutor::Run (void)
{
	assert (m_bInitialized);

	for (;;)
	{
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

		Event PendingEvent;
		if (m_pEventQueue->Pop (PendingEvent))
		{
			for (unsigned nService = 0; nService < m_nServiceCount; nService++)
			{
				m_pServices[nService]->OnEvent (PendingEvent);
			}
		}
	}
}
