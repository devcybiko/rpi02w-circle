#include "loggerservice.h"

#include <circle/util.h>
#include <assert.h>

CLoggerService::CLoggerService (unsigned nLogLevel, CTimer *pTimer,
				CDeviceNameService *pDeviceNameService,
				const char *pLogDevice, CDevice *pFallbackDevice)
: CService (PeriodDisabled),
  m_pDeviceNameService (pDeviceNameService),
  m_pLogDevice (pLogDevice),
  m_pFallbackDevice (pFallbackDevice),
  m_Logger (nLogLevel, pTimer)
{
	assert (pTimer != 0);
	assert (m_pDeviceNameService != 0);
	assert (m_pLogDevice != 0);
	assert (m_pFallbackDevice != 0);
}

CLoggerService::~CLoggerService (void)
{
	m_pFallbackDevice = 0;
	m_pLogDevice = 0;
	m_pDeviceNameService = 0;
}

boolean CLoggerService::InitService (void)
{
	CDevice *pTarget = m_pDeviceNameService->GetDevice (m_pLogDevice, FALSE);
	return m_Logger.Initialize (pTarget != 0 ? pTarget : m_pFallbackDevice);
}

void CLoggerService::Update (void)
{
}

void CLoggerService::OnEvent (const Event &event)
{
	if (event.type != EventType::Logger || event.logger.level > LogDebug)
	{
		return;
	}

	char Message[sizeof event.logger.message + 1];
	memcpy (Message, event.logger.message, sizeof event.logger.message);
	Message[sizeof event.logger.message] = '\0';

	// m_Logger.Write ("logger_service",
	// 		static_cast<TLogSeverity> (event.logger.level), "%s", Message);
}

CLogger *CLoggerService::GetLogger (void)
{
	return &m_Logger;
}
