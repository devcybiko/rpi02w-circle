#include "networkservice.h"

#include <circle/logger.h>
#include <circle/string.h>

static const char FromNetworkService[] = "network";

CNetworkService::CNetworkService (void)
: CService (PeriodPolling),
  m_Net (0, 0, 0, 0, DEFAULT_HOSTNAME, NetDeviceTypeWLAN),
  m_bReportedNetwork (FALSE)
{
}

CNetworkService::~CNetworkService (void)
{
}

boolean CNetworkService::InitService (void)
{
	m_bReportedNetwork = FALSE;
	return m_Net.Initialize (FALSE);
}

void CNetworkService::Update (void)
{
	if (!m_bReportedNetwork && m_Net.IsRunning ())
	{
		CString IPAddress;
		m_Net.GetConfig ()->GetIPAddress ()->Format (&IPAddress);
		CLogger::Get ()->Write (FromNetworkService, LogNotice,
				 "WLAN connected: %s", static_cast<const char *> (IPAddress));
		m_bReportedNetwork = TRUE;
	}
}

void CNetworkService::OnEvent (const Event &)
{
}

boolean CNetworkService::IsRunning (void) const
{
	return m_Net.IsRunning ();
}

CNetSubSystem *CNetworkService::GetNetSubSystem (void)
{
	return &m_Net;
}
