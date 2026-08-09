#pragma once

#include "eventqueue.h"
#include "service.h"

#include <circle/types.h>

class CExecutor
{
public:
	static constexpr unsigned MaxServices = 64;

	explicit CExecutor (CEventQueue *pEventQueue);

	boolean RegisterService (CService *pService);
	boolean Initialize (void);
	void Run (void);

private:
	CEventQueue *m_pEventQueue;
	CService *m_pServices[MaxServices];
	unsigned m_nServiceCount;
	boolean m_bInitialized;
};
