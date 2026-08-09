#include "service.h"

#include <circle/timer.h>

static constexpr u64 TimeoutDisabled = ~static_cast<u64> (0);

CService::CService (u32 nPeriodMS)
: m_periodMS (nPeriodMS),
  m_timeout (TimeoutDisabled)
{
}

CService::~CService (void)
{
}

boolean CService::InitService (void)
{
	return TRUE;
}

void CService::ResetTimer (void)
{
	if (m_periodMS == PeriodPolling)
	{
		m_timeout = 0;
	}
	else if (m_periodMS == PeriodDisabled)
	{
		m_timeout = TimeoutDisabled;
	}
	else
	{
		m_timeout = CTimer::GetClockTicks64 () + static_cast<u64> (m_periodMS) * 1000;
	}
}

boolean CService::IsUpdateDue (u64 nNow) const
{
	return m_timeout != TimeoutDisabled && nNow >= m_timeout;
}
