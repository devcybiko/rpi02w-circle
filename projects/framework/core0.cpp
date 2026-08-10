#include "core0.h"
#include "drawservice.h"
#include "hidservice.h"
#include "loggerservice.h"
#include "screenservice.h"
#include "serialservice.h"
#include "storageservice.h"
#include "webserverservice.h"

#define DRIVE		"SD:"
#define FIRMWARE_PATH	DRIVE "/firmware/"
#define CONFIG_FILE	DRIVE "/wpa_supplicant.conf"
#define WEB_ROOT	DRIVE "/www"

CCore0::CCore0 (CEventRouter *pEventRouter)
: CCore (pEventRouter, 0, FALSE),
  m_Timer (&m_Interrupt)
{
	m_Dispatcher.SetScheduler (&m_Scheduler);
	m_ActLED.Blink (5);
}

CCore0::~CCore0 (void)
{
	m_Dispatcher.RemoveServices ();
}

boolean CCore0::Initialize (void)
{
	boolean bOK = TRUE;
	bOK = m_Interrupt.Initialize ();
	if (bOK) bOK = m_Timer.Initialize ();

	CScreenService *pScreenService = 0;
	CScreenDevice *pScreen = 0;
	if (bOK)
	{
		pScreenService = new CScreenService (m_Options.GetWidth (),
						     m_Options.GetHeight ());
		pScreen = pScreenService->GetScreen ();
		bOK = m_Dispatcher.AddService (pScreenService);
	}

	if (bOK) bOK = m_Dispatcher.AddService (new CDrawService (pScreenService));
	if (bOK) bOK = m_Dispatcher.AddService (new CSerialService (115200));
	if (bOK) bOK = m_Dispatcher.AddService (new CLoggerService (
		m_Options.GetLogLevel (), &m_Timer, &m_DeviceNameService,
		m_Options.GetLogDevice (), pScreen));
	if (bOK) bOK = m_Dispatcher.AddService (new CHIDService (
		&m_Interrupt, &m_Timer, &m_DeviceNameService, pScreen));
	if (bOK) bOK = m_Dispatcher.AddService (new CStorageService (
		&m_Interrupt, &m_Timer, &m_ActLED, DRIVE));
	if (bOK) bOK = m_Dispatcher.AddService (new CWebServerService (
		FIRMWARE_PATH, CONFIG_FILE, &m_Timer, pScreen, WEB_ROOT));

	for (unsigned nType = static_cast<unsigned> (EventType::None) + 1;
	     bOK && nType < static_cast<unsigned> (EventType::Count); nType++)
	{
		bOK = Subscribe (static_cast<EventType> (nType));
	}

	return bOK;
}
