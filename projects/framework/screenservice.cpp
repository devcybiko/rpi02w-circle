#include "screenservice.h"

CScreenService::CScreenService (unsigned nWidth, unsigned nHeight)
: CService (PeriodDisabled),
  m_Screen (nWidth, nHeight, Font12x22),
  m_pGraphics (0)
{
}

CScreenService::~CScreenService (void)
{
	delete m_pGraphics;
	m_pGraphics = 0;
}

boolean CScreenService::InitService (void)
{
	if (!m_Screen.Initialize ())
	{
		return FALSE;
	}

	m_pGraphics = new C2DGraphics (m_Screen.GetFrameBuffer ());
	if (m_pGraphics == 0 || !m_pGraphics->Initialize ())
	{
		delete m_pGraphics;
		m_pGraphics = 0;
		return FALSE;
	}

	m_pGraphics->ClearScreen (CDisplay::Black);
	m_pGraphics->UpdateDisplay ();
	return TRUE;
}

void CScreenService::Update (void)
{
}

void CScreenService::OnEvent (const Event &)
{
}

CScreenDevice *CScreenService::GetScreen (void)
{
	return &m_Screen;
}

C2DGraphics *CScreenService::GetGraphics (void)
{
	return m_pGraphics;
}
