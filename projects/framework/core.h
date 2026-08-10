#pragma once

#include "dispatcher.h"
#include "eventqueue.h"

#include <circle/types.h>

class CEventRouter;
class CCore;

extern CCore *pCore0;
extern CCore *pCore1;
extern CCore *pCore2;
extern CCore *pCore3;

class CCore
{
public:
	CCore (CEventRouter *pEventRouter, unsigned nCore, boolean bNeedsWakeUp);
	virtual ~CCore (void);

	static CCore *GetCore (unsigned nCore);

	boolean Post (const Event &EventToPost);
	u32 Publish (const Event &EventToPublish);
	virtual void Run (void);
	CService *FindService (const char *pName)
	{
		return m_Dispatcher.FindService (pName);
	}

protected:
	boolean Subscribe (EventType Type);
	boolean Unsubscribe (EventType Type);
	void ProcessEvents (void);

private:
	static CCore *s_pCores[];

	CEventRouter *m_pEventRouter;
	unsigned m_nCore;
	CEventQueue m_EventQueue;

protected:
	CDispatcher m_Dispatcher;
};
