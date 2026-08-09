#include "wlanservice.h"

#include <assert.h>

CWLANService::CWLANService (const char *pFirmwarePath)
: CService (PeriodDisabled),
  m_pFirmwarePath (pFirmwarePath),
  m_WLAN (pFirmwarePath)
{
	assert (m_pFirmwarePath != 0);
}

CWLANService::~CWLANService (void)
{
	m_pFirmwarePath = 0;
}

boolean CWLANService::InitService (void)
{
	return m_WLAN.Initialize ();
}

void CWLANService::Update (void)
{
}

void CWLANService::OnEvent (const Event &)
{
}
