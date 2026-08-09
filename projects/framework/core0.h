#ifndef _framework_core0_h
#define _framework_core0_h

#include "core.h"
#include "hidservice.h"
#include "loggerservice.h"
#include "screenservice.h"
#include "serialservice.h"
#include "storageservice.h"
#include "webserverservice.h"

#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/sched/scheduler.h>
#include <circle/types.h>

class CCore0 : public CCore
{
public:
	explicit CCore0 (CEventRouter *pEventRouter);
	~CCore0 (void);

	boolean Initialize (void);
	void Run (void) override;

private:
	void HandleEvent (const Event &EventToHandle) override;

private:
	// Initialization order is significant: dependencies must precede their users.
	CActLED			m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CScreenService		m_ScreenService;
	CSerialService		m_SerialService;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CTimer			m_Timer;
	CLoggerService		m_LoggerService;
	CScheduler		m_Scheduler;
	CHIDService		m_HIDService;
	CStorageService		m_StorageService;
	CWebServerService	m_WebServerService;
};

#endif
