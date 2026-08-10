#include "drawservice.h"

#include <circle/input/mousebehaviour.h>
#include <assert.h>

CDrawService::CDrawService (CScreenService *pScreenService)
: CService (PeriodDisabled),
  m_bDrawing (FALSE),
  m_nLastX (0),
  m_nLastY (0),
  m_pScreenService (pScreenService),
  m_pGraphics (0)
{
	assert (m_pScreenService != 0);
}

CDrawService::~CDrawService (void)
{
	m_pGraphics = 0;
	m_pScreenService = 0;
}

boolean CDrawService::InitService (void)
{
	m_pGraphics = m_pScreenService->GetGraphics ();
	return m_pGraphics != 0;
}

void CDrawService::Update (void)
{
}

void CDrawService::OnEvent (const Event &EventToHandle)
{
	if (EventToHandle.type == EventType::Key)
	{
		if (EventToHandle.key.pressed
		    && (EventToHandle.key.key == 'c' || EventToHandle.key.key == 'C'))
		{
			m_pGraphics->ClearScreen (CDisplay::Black);
			m_pGraphics->UpdateDisplay ();
			m_bDrawing = FALSE;
		}
		return;
	}

	if (EventToHandle.type != EventType::Mouse)
	{
		return;
	}

	const int nX = EventToHandle.mouse.x;
	const int nY = EventToHandle.mouse.y;
	const boolean bInBounds = nX >= 0 && nY >= 0
		&& static_cast<unsigned> (nX) < m_pGraphics->GetWidth ()
		&& static_cast<unsigned> (nY) < m_pGraphics->GetHeight ();

	if (!m_bDrawing)
	{
		if (EventToHandle.mouse.event == MouseEventMouseDown
		    && EventToHandle.mouse.buttons != 0 && bInBounds)
		{
			m_bDrawing = TRUE;
			m_nLastX = nX;
			m_nLastY = nY;
			m_pGraphics->DrawLine (nX, nY, nX, nY, CDisplay::White);
			m_pGraphics->UpdateDisplay ();
		}
		return;
	}

	if (bInBounds)
	{
		m_pGraphics->DrawLine (m_nLastX, m_nLastY, nX, nY,
				       CDisplay::White);
		m_pGraphics->UpdateDisplay ();
		m_nLastX = nX;
		m_nLastY = nY;
	}

	if (EventToHandle.mouse.buttons == 0)
	{
		m_bDrawing = FALSE;
	}
}
