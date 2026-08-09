#pragma once

#include "eventqueue.h"

#include <circle/types.h>

class CCore
{
public:
	explicit CCore (unsigned nCore);
	virtual ~CCore (void);

	virtual void Run (void);
	boolean QueueEvent (const Event &EventToQueue);

protected:
	void ProcessEvents (void);
	virtual void HandleEvent (const Event &EventToHandle) = 0;

private:
	unsigned m_nCore;
	CEventQueue m_EventQueue;
};
