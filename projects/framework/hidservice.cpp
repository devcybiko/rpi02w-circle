#include "hidservice.h"
#include "core.h"

#include <circle/logger.h>
#include <circle/util.h>
#include <assert.h>

static const char FromHIDService[] = "hid";

CHIDService *CHIDService::s_pThis = 0;

CHIDService::CHIDService (CInterruptSystem *pInterruptSystem, CTimer *pTimer,
			  CDeviceNameService *pDeviceNameService, CScreenDevice *pScreen)
: CService (PeriodPolling),
  m_pDeviceNameService (pDeviceNameService),
  m_pScreen (pScreen),
  m_USBHCI (pInterruptSystem, pTimer, TRUE),
  m_pKeyboard (0),
  m_pMouse (0)
{
	assert (s_pThis == 0);
	assert (pInterruptSystem != 0);
	assert (pTimer != 0);
	assert (m_pDeviceNameService != 0);
	assert (m_pScreen != 0);

	s_pThis = this;
}

CHIDService::~CHIDService (void)
{
	s_pThis = 0;
	m_pMouse = 0;
	m_pKeyboard = 0;
	m_pScreen = 0;
	m_pDeviceNameService = 0;
}

boolean CHIDService::InitService (void)
{
	if (!m_USBHCI.Initialize ())
	{
		CLogger::Get ()->Write (FromHIDService, LogError,
				       "Cannot initialize USB host controller");
		return FALSE;
	}

	Update ();
	if (m_pMouse == 0)
	{
		CLogger::Get ()->Write (FromHIDService, LogNotice,
				       "USB mouse is not initialized");
	}

	return TRUE;
}

void CHIDService::Update (void)
{
	// Must run at task level to detect devices connected after boot.
	if (m_USBHCI.UpdatePlugAndPlay ())
	{
		CLogger::Get ()->Write (FromHIDService, LogNotice,
				       "USB topology changed; registered devices follow");
		m_pDeviceNameService->EnumerateDevices (LogDevice, 0);
	}

	AttachKeyboard ();
	AttachMouse ();

	if (m_pKeyboard != 0)
	{
		m_pKeyboard->UpdateLEDs ();
	}

	if (m_pMouse != 0)
	{
		m_pMouse->UpdateCursor ();
	}
}

void CHIDService::OnEvent (const Event &)
{
	// HID input currently originates from the USB callbacks.
}

void CHIDService::AttachKeyboard (void)
{
	if (m_pKeyboard != 0)
	{
		return;
	}

	m_pKeyboard = static_cast<CUSBKeyboardDevice *> (
		m_pDeviceNameService->GetDevice ("ukbd1", FALSE));

	if (m_pKeyboard != 0)
	{
		m_pKeyboard->RegisterRemovedHandler (KeyboardRemovedHandler);
		m_pKeyboard->RegisterKeyPressedHandler (KeyPressedHandler);
		CLogger::Get ()->Write (FromHIDService, LogNotice, "USB keyboard attached");
	}
}

void CHIDService::AttachMouse (void)
{
	if (m_pMouse != 0)
	{
		return;
	}

	m_pMouse = static_cast<CMouseDevice *> (
		m_pDeviceNameService->GetDevice ("mouse1", FALSE));

	if (m_pMouse != 0)
	{
		m_pMouse->RegisterRemovedHandler (MouseRemovedHandler);

		boolean bMouseInitialized = m_pMouse->Setup (m_pScreen->GetFrameBuffer ());
		if (!bMouseInitialized)
		{
			CLogger::Get ()->Write (FromHIDService, LogWarning,
					       "Cannot set up USB mouse cursor; retrying without cursor");

			// Setup() retains display state when firmware cursor setup fails.
			// Release it before retrying cooked mode without a hardware cursor.
			m_pMouse->Release ();
			bMouseInitialized = m_pMouse->Setup (m_pScreen->GetFrameBuffer (), FALSE);
		}

		if (bMouseInitialized)
		{
			m_pMouse->SetCursor (m_pScreen->GetWidth () / 2,
					     m_pScreen->GetHeight () / 2);
			m_pMouse->ShowCursor (TRUE);
			m_pMouse->RegisterEventHandler (MouseEventHandler);
			CLogger::Get ()->Write (FromHIDService, LogNotice,
					       "USB mouse attached");
		}
		else
		{
			CLogger::Get ()->Write (FromHIDService, LogError,
					       "USB mouse is not initialized");
		}
	}
}

void CHIDService::KeyPressedHandler (const char *pString)
{
	assert (s_pThis != 0);
	pCore0->Publish (Event::Key (*pString, TRUE));
}

void CHIDService::KeyboardRemovedHandler (CDevice *, void *)
{
	assert (s_pThis != 0);
	s_pThis->m_pKeyboard = 0;
	CLogger::Get ()->Write (FromHIDService, LogNotice, "USB keyboard removed");
}

void CHIDService::MouseRemovedHandler (CDevice *, void *)
{
	assert (s_pThis != 0);
	s_pThis->m_pMouse = 0;
	CLogger::Get ()->Write (FromHIDService, LogNotice, "USB mouse removed");
}

void CHIDService::MouseEventHandler (TMouseEvent Event, unsigned nButtons,
				     unsigned nPosX, unsigned nPosY, int nWheelMove)
{
	pCore0->Publish (Event::Mouse (Event, nButtons, nPosX, nPosY, nWheelMove));
}

boolean CHIDService::LogDevice (CDevice *pDevice, const char *pName,
				boolean bBlockDevice, void *)
{
	assert (pDevice != 0);
	assert (pName != 0);

	const char *pVendor = pDevice->GetProperty (CDevice::PropertyVendor);
	const char *pProduct = pDevice->GetProperty (CDevice::PropertyProduct);

	CLogger::Get ()->Write (FromHIDService, LogNotice,
			       "device: name=%s, type=%s, vendor=%s, product=%s",
			       pName, bBlockDevice ? "block" : "character",
			       pVendor != 0 && *pVendor != '\0' ? pVendor : "<unknown>",
			       pProduct != 0 && *pProduct != '\0' ? pProduct : "<unknown>");

	return TRUE;
}
