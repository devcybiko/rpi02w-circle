#include "usbhostservice.h"

#include <assert.h>

CUSBHostService::CUSBHostService (CInterruptSystem *pInterruptSystem, CTimer *pTimer)
: CService (PeriodPolling),
  m_USBHCI (pInterruptSystem, pTimer, TRUE)
{
	assert (pInterruptSystem != 0);
	assert (pTimer != 0);
}

CUSBHostService::~CUSBHostService (void)
{
}

boolean CUSBHostService::InitService (void)
{
	return m_USBHCI.Initialize ();
}

void CUSBHostService::Update (void)
{
	// Must run at task level to detect devices connected after boot.
	m_USBHCI.UpdatePlugAndPlay ();
}

void CUSBHostService::OnEvent (const Event &)
{
}
