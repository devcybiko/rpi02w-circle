#ifndef _framework_core0_h
#define _framework_core0_h

#include "core.h"

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

private:
	// Initialization order is significant: dependencies must precede their users.
	CActLED			m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CTimer			m_Timer;
	CScheduler		m_Scheduler;
};

#endif
