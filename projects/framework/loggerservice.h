#pragma once

#include "service.h"

#include <circle/device.h>
#include <circle/devicenameservice.h>
#include <circle/logger.h>
#include <circle/timer.h>

class CLoggerService : public CService
{
public:
	CLoggerService (unsigned nLogLevel, CTimer *pTimer,
			CDeviceNameService *pDeviceNameService,
			const char *pLogDevice, CDevice *pFallbackDevice);
	~CLoggerService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

	CLogger *GetLogger (void);

private:
	CDeviceNameService *m_pDeviceNameService;
	const char *m_pLogDevice;
	CDevice *m_pFallbackDevice;
	CLogger m_Logger;
};
