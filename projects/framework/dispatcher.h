#pragma once

#include "eventqueue.h"
#include "service.h"

#include <circle/types.h>

class CScheduler;

class CDispatcher
{
public:
	static constexpr unsigned MaxServices = 64;

	explicit CDispatcher (CEventQueue *pEventQueue, CScheduler *pScheduler = 0);
	~CDispatcher (void);

	void SetScheduler (CScheduler *pScheduler);
	boolean AddService (CService *pService);
	void RemoveServices (void);
	boolean InitServices (void);
	void UpdateServices (void);
	void Dispatch (const Event &EventToDispatch);
	CService *FindService (const char *pName);

private:
	CEventQueue *m_pEventQueue;
	CScheduler *m_pScheduler;
	CService *m_pServices[MaxServices];
	unsigned m_nServiceCount;
	boolean m_bInitialized;
};
