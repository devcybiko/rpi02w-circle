#pragma once

#include "event.h"

#include <circle/spinlock.h>
#include <circle/types.h>

class CEventQueue
{
public:
	static constexpr unsigned Capacity = 64;

	CEventQueue (void);

	// Returns FALSE when the bounded queue is full.
	boolean Push (const Event &EventToPush);

	// Must only be called by this queue's single consumer.
	boolean Pop (Event &EventToPop);

private:
	Event m_Events[Capacity];
	unsigned m_nRead;
	unsigned m_nWrite;
	unsigned m_nCount;
	CSpinLock m_Lock;
};
