#include "screenservice.h"

CScreenService::CScreenService (unsigned nWidth, unsigned nHeight)
: CService (PeriodDisabled),
  m_Screen (nWidth, nHeight, Font12x22)
{
}

CScreenService::~CScreenService (void)
{
}

boolean CScreenService::InitService (void)
{
	return m_Screen.Initialize ();
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
