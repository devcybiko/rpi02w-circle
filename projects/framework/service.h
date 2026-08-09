#pragma once

#include "event.h"

#include <circle/types.h>

class CService
{
public:
	static constexpr u32 PeriodPolling = 0;
	static constexpr u32 PeriodDisabled = ~static_cast<u32> (0);

	explicit CService (u32 nPeriodMS);
	virtual ~CService (void);

	virtual boolean InitService (void);
	virtual void Update (void) = 0;
	virtual void OnEvent (const Event &EventToHandle) = 0;

protected:
	// Period in milliseconds, or one of the Period* sentinels above.
	u32 m_periodMS;

	// Absolute deadline in Circle's 1 MHz clock ticks.
	u64 m_timeout;

private:
	void ResetTimer (void);
	boolean IsUpdateDue (u64 nNow) const;

	friend class CExecutor;
};
