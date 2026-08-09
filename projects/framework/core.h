#pragma once

#include "eventqueue.h"

#include <circle/types.h>

class CEventRouter;

class CCore
{
public:
	CCore (CEventRouter *pEventRouter, unsigned nCore, boolean bNeedsWakeUp);
	virtual ~CCore (void);

	virtual void Run (void);

protected:
	boolean SendEventToCore (unsigned nCore, const Event &EventToSend);
	boolean Subscribe (EventType Type);
	boolean Unsubscribe (EventType Type);
	u32 PublishEvent (const Event &EventToPublish);
	void ProcessEvents (void);
	virtual void HandleEvent (const Event &EventToHandle) = 0;

private:
	CEventRouter *m_pEventRouter;
	unsigned m_nCore;
	CEventQueue m_EventQueue;
};
