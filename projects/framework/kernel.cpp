#include "kernel.h"
#include "webserver.h"

#include <assert.h>
#include <circle/string.h>
#include <circle/util.h>

#define DRIVE		"SD:"
#define FIRMWARE_PATH	DRIVE "/firmware/"
#define CONFIG_FILE	DRIVE "/wpa_supplicant.conf"
#define WEB_ROOT	DRIVE "/www"

static const char FromKernel[] = "framework";

CKernel *CKernel::s_pThis = 0;

CKernel::CKernel (void)
: m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
  m_Timer (&m_Interrupt),
  m_Logger (m_Options.GetLogLevel (), &m_Timer),
  m_USBHCI (&m_Interrupt, &m_Timer, TRUE),
  m_EMMC (&m_Interrupt, &m_Timer, &m_ActLED),
  m_WLAN (FIRMWARE_PATH),
  m_Net (0, 0, 0, 0, DEFAULT_HOSTNAME, NetDeviceTypeWLAN),
  m_WPASupplicant (CONFIG_FILE),
  m_pKeyboard (0),
  m_pMouse (0)
{
	s_pThis = this;
	m_ActLED.Blink (5);
}

CKernel::~CKernel (void)
{
	s_pThis = 0;
}

boolean CKernel::Initialize (void)
{
	boolean bOK = m_Screen.Initialize ();

	if (bOK) bOK = m_Serial.Initialize (115200);
	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		bOK = m_Logger.Initialize (pTarget != 0 ? pTarget : &m_Screen);
	}
	if (bOK) bOK = m_Interrupt.Initialize ();
	if (bOK) bOK = m_Timer.Initialize ();
	if (bOK) bOK = m_USBHCI.Initialize ();
	if (bOK) bOK = m_EMMC.Initialize ();
	if (bOK && f_mount (&m_FileSystem, DRIVE, 1) != FR_OK)
	{
		m_Logger.Write (FromKernel, LogError, "Cannot mount %s", DRIVE);
		bOK = FALSE;
	}
	if (bOK) bOK = m_WLAN.Initialize ();
	if (bOK) bOK = m_Net.Initialize (FALSE);
	if (bOK) bOK = m_WPASupplicant.Initialize ();

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Base framework built " __DATE__ " " __TIME__);
	m_Logger.Write (FromKernel, LogNotice, "HDMI, SD, WLAN, USB keyboard and mouse are enabled");

	boolean bReportedNetwork = FALSE;
	boolean bWebServerStarted = FALSE;
	for (;;)
	{
		// Must run at task level to detect devices connected after boot.
		if (m_USBHCI.UpdatePlugAndPlay ())
		{
			AttachInputDevices ();
		}

		if (m_pKeyboard != 0)
		{
			m_pKeyboard->UpdateLEDs ();
		}
		if (m_pMouse != 0)
		{
			m_pMouse->UpdateCursor ();
		}

		if (!bReportedNetwork && m_Net.IsRunning ())
		{
			CString IPAddress;
			m_Net.GetConfig ()->GetIPAddress ()->Format (&IPAddress);
			m_Logger.Write (FromKernel, LogNotice, "WLAN connected: %s", (const char *) IPAddress);
			bReportedNetwork = TRUE;
		}
		if (!bWebServerStarted && m_Net.IsRunning ())
		{
			new CSDWebServer (&m_Net, WEB_ROOT, &m_Timer, &m_Screen);
			m_Logger.Write (FromKernel, LogNotice, "HTTP server listening on port 80 (root %s)", WEB_ROOT);
			bWebServerStarted = TRUE;
		}

		m_Scheduler.MsSleep (10);
	}
}

void CKernel::AttachInputDevices (void)
{
	if (m_pKeyboard == 0)
	{
		m_pKeyboard = (CUSBKeyboardDevice *) m_DeviceNameService.GetDevice ("ukbd1", FALSE);
		if (m_pKeyboard != 0)
		{
			m_pKeyboard->RegisterRemovedHandler (KeyboardRemovedHandler);
			m_pKeyboard->RegisterKeyPressedHandler (KeyPressedHandler);
			m_Logger.Write (FromKernel, LogNotice, "USB keyboard attached");
		}
	}

	if (m_pMouse == 0)
	{
		m_pMouse = (CMouseDevice *) m_DeviceNameService.GetDevice ("mouse1", FALSE);
		if (m_pMouse != 0)
		{
			m_pMouse->RegisterRemovedHandler (MouseRemovedHandler);
			if (m_pMouse->Setup (m_Screen.GetFrameBuffer ()))
			{
				m_pMouse->SetCursor (m_Screen.GetWidth () / 2, m_Screen.GetHeight () / 2);
				m_pMouse->ShowCursor (TRUE);
				m_pMouse->RegisterEventHandler (MouseEventHandler);
			}
			m_Logger.Write (FromKernel, LogNotice, "USB mouse attached");
		}
	}
}

void CKernel::KeyPressedHandler (const char *pString)
{
	assert (s_pThis != 0);
	s_pThis->m_Screen.Write (pString, strlen (pString));
}

void CKernel::KeyboardRemovedHandler (CDevice *, void *)
{
	assert (s_pThis != 0);
	s_pThis->m_pKeyboard = 0;
	CLogger::Get ()->Write (FromKernel, LogNotice, "USB keyboard removed");
}

void CKernel::MouseRemovedHandler (CDevice *, void *)
{
	assert (s_pThis != 0);
	s_pThis->m_pMouse = 0;
	CLogger::Get ()->Write (FromKernel, LogNotice, "USB mouse removed");
}

void CKernel::MouseEventHandler (TMouseEvent, unsigned, unsigned, unsigned, int)
{
	// The CMouseDevice draws and updates its framebuffer cursor itself.
}
