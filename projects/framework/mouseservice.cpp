#include "mouseservice.h"

#include <circle/logger.h>
#include <assert.h>

static const char FromMouseService[] = "mouse";

CMouseService *CMouseService::s_pThis = 0;

CMouseService::CMouseService (CDeviceNameService *pDeviceNameService,
			      CScreenDevice *pScreen)
: CService (PeriodPolling),
  m_pDeviceNameService (pDeviceNameService),
  m_pScreen (pScreen),
  m_pMouse (0)
{
	assert (s_pThis == 0);
	assert (m_pDeviceNameService != 0);
	assert (m_pScreen != 0);

	s_pThis = this;
}

CMouseService::~CMouseService (void)
{
	s_pThis = 0;
	m_pMouse = 0;
	m_pScreen = 0;
	m_pDeviceNameService = 0;
}

boolean CMouseService::InitService (void)
{
	// USB has been initialized by core 0 before this method is called.
	Update ();

	return TRUE;
}

void CMouseService::Update (void)
{
	if (m_pMouse == 0)
	{
		m_pMouse = static_cast<CMouseDevice *> (
			m_pDeviceNameService->GetDevice ("mouse1", FALSE));

		if (m_pMouse != 0)
		{
			m_pMouse->RegisterRemovedHandler (MouseRemovedHandler);
			if (m_pMouse->Setup (m_pScreen->GetFrameBuffer ()))
			{
				m_pMouse->SetCursor (m_pScreen->GetWidth () / 2,
						     m_pScreen->GetHeight () / 2);
				m_pMouse->ShowCursor (TRUE);
				m_pMouse->RegisterEventHandler (MouseEventHandler);
			}

			CLogger::Get ()->Write (FromMouseService, LogNotice,
					       "USB mouse attached");
		}
	}

	if (m_pMouse != 0)
	{
		m_pMouse->UpdateCursor ();
	}
}

void CMouseService::OnEvent (const Event &)
{
	// Mouse input currently originates from the USB callback.
}

void CMouseService::MouseRemovedHandler (CDevice *, void *)
{
	assert (s_pThis != 0);
	s_pThis->m_pMouse = 0;
	CLogger::Get ()->Write (FromMouseService, LogNotice, "USB mouse removed");
}

void CMouseService::MouseEventHandler (TMouseEvent, unsigned, unsigned, unsigned, int)
{
	// CMouseDevice draws and updates its framebuffer cursor itself.
}
