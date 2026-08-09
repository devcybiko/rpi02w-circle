#include "core0.h"

#define DRIVE		"SD:"
#define FIRMWARE_PATH	DRIVE "/firmware/"
#define CONFIG_FILE	DRIVE "/wpa_supplicant.conf"
#define WEB_ROOT	DRIVE "/www"

static const char FromCore0[] = "core0";

CCore0::CCore0 (CEventRouter *pEventRouter)
: CCore (pEventRouter, 0, FALSE),
  m_ScreenService (m_Options.GetWidth (), m_Options.GetHeight ()),
  m_SerialService (115200),
  m_Timer (&m_Interrupt),
  m_LoggerService (m_Options.GetLogLevel (), &m_Timer, &m_DeviceNameService,
		   m_Options.GetLogDevice (), m_ScreenService.GetScreen ()),
  m_HIDService (&m_Interrupt, &m_Timer, &m_DeviceNameService,
		m_ScreenService.GetScreen ()),
  m_StorageService (&m_Interrupt, &m_Timer, &m_ActLED, DRIVE),
  m_WebServerService (FIRMWARE_PATH, CONFIG_FILE, &m_Timer,
		      m_ScreenService.GetScreen (), WEB_ROOT)
{
	m_ActLED.Blink (5);
}

CCore0::~CCore0 (void)
{
}

boolean CCore0::Initialize (void)
{
	boolean bOK = m_ScreenService.InitService ();

	if (bOK) bOK = m_SerialService.InitService ();
	if (bOK) bOK = m_LoggerService.InitService ();
	if (bOK) bOK = m_Interrupt.Initialize ();
	if (bOK) bOK = m_Timer.Initialize ();
	if (bOK) bOK = m_HIDService.InitService ();
	if (bOK) bOK = m_StorageService.InitService ();
	if (bOK) bOK = m_WebServerService.InitService ();

	return bOK;
}

void CCore0::Run (void)
{
	m_LoggerService.GetLogger ()->Write (FromCore0, LogNotice,
				      "Base framework built " __DATE__ " " __TIME__);
	m_LoggerService.GetLogger ()->Write (FromCore0, LogNotice,
				      "HDMI, SD, WLAN, USB keyboard and mouse are enabled");

	for (;;)
	{
		ProcessEvents ();

		m_HIDService.Update ();
		m_WebServerService.Update ();

		m_Scheduler.MsSleep (10);
	}
}

void CCore0::HandleEvent (const Event &)
{
	// Core 0 event handling will be added as event producers are connected.
}
