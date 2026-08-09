#ifndef _framework_core0_h
#define _framework_core0_h

#include "core.h"
#include "keyboardservice.h"
#include "mouseservice.h"
#include "networkservice.h"
#include "screenservice.h"
#include "usbhostservice.h"
#include "webserverservice.h"

#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/sched/scheduler.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>
#include <wlan/bcm4343.h>
#include <wlan/hostap/wpa_supplicant/wpasupplicant.h>
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
	CSerialDevice		m_Serial;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CTimer			m_Timer;
	CLogger			m_Logger;
	CScheduler		m_Scheduler;
	CUSBHostService		m_USBHostService;
	CEMMCDevice		m_EMMC;
	FATFS			m_FileSystem;
	CBcm4343Device		m_WLAN;
	CNetworkService		m_NetworkService;
	CWPASupplicant		m_WPASupplicant;
	CKeyboardService	m_KeyboardService;
	CMouseService		m_MouseService;
	CWebServerService	m_WebServerService;
};

#endif
