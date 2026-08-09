#include "keyboardservice.h"

#include <circle/logger.h>
#include <circle/util.h>
#include <assert.h>

static const char FromKeyboardService[] = "keyboard";

CKeyboardService *CKeyboardService::s_pThis = 0;

CKeyboardService::CKeyboardService (CDeviceNameService *pDeviceNameService,
				    CScreenDevice *pScreen)
: CService (PeriodPolling),
  m_pDeviceNameService (pDeviceNameService),
  m_pScreen (pScreen),
  m_pKeyboard (0)
{
	assert (s_pThis == 0);
	assert (m_pDeviceNameService != 0);
	assert (m_pScreen != 0);

	s_pThis = this;
}

CKeyboardService::~CKeyboardService (void)
{
	s_pThis = 0;
	m_pKeyboard = 0;
	m_pScreen = 0;
	m_pDeviceNameService = 0;
}

boolean CKeyboardService::InitService (void)
{
	// USB has been initialized by core 0 before this method is called.
	// Update() performs the initial discovery and remains responsible for
	// detecting a keyboard connected later.
	Update ();

	return TRUE;
}

void CKeyboardService::Update (void)
{
	if (m_pKeyboard == 0)
	{
		m_pKeyboard = static_cast<CUSBKeyboardDevice *> (
			m_pDeviceNameService->GetDevice ("ukbd1", FALSE));

		if (m_pKeyboard != 0)
		{
			m_pKeyboard->RegisterRemovedHandler (KeyboardRemovedHandler);
			m_pKeyboard->RegisterKeyPressedHandler (KeyPressedHandler);
			CLogger::Get ()->Write (FromKeyboardService, LogNotice,
					       "USB keyboard attached");
		}
	}

	if (m_pKeyboard != 0)
	{
		m_pKeyboard->UpdateLEDs ();
	}
}

void CKeyboardService::OnEvent (const Event &)
{
	// Keyboard input currently originates from the USB callback.
}

void CKeyboardService::KeyPressedHandler (const char *pString)
{
	assert (s_pThis != 0);
	s_pThis->m_pScreen->Write (pString, strlen (pString));
}

void CKeyboardService::KeyboardRemovedHandler (CDevice *, void *)
{
	assert (s_pThis != 0);
	s_pThis->m_pKeyboard = 0;
	CLogger::Get ()->Write (FromKeyboardService, LogNotice,
				 "USB keyboard removed");
}
