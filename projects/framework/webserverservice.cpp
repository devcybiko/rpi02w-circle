#include "webserverservice.h"

#include <circle/logger.h>
#include <circle/string.h>
#include <assert.h>
#include <string.h>

static const char FromWebServerService[] = "webserver";

CWebServerService::CWebServerService (const char *pFirmwarePath,
				      const char *pConfigFile, CTimer *pTimer,
				      CScreenDevice *pScreen, const char *pWebRoot)
: CService (PeriodPolling),
  m_pFirmwarePath (pFirmwarePath),
  m_pConfigFile (pConfigFile),
  m_pTimer (pTimer),
  m_pScreen (pScreen),
  m_pWebRoot (pWebRoot),
  m_WLAN (pFirmwarePath),
  m_Net (0, 0, 0, 0, DEFAULT_HOSTNAME, NetDeviceTypeWLAN),
  m_WPASupplicant (pConfigFile),
  m_bReportedNetwork (FALSE),
  m_pWebServer (0)
{
	assert (m_pFirmwarePath != 0);
	assert (m_pConfigFile != 0);
	assert (m_pTimer != 0);
	assert (m_pScreen != 0);
	assert (m_pWebRoot != 0);
	strcpy (m_name, "webserver");
}

CWebServerService::~CWebServerService (void)
{
	// CSDWebServer is a process-lifetime scheduler task. It is intentionally
	// not deleted while the scheduler may still run it.
	m_pWebServer = 0;
	m_pWebRoot = 0;
	m_pScreen = 0;
	m_pTimer = 0;
	m_pConfigFile = 0;
	m_pFirmwarePath = 0;
}

boolean CWebServerService::InitService (void)
{
	if (!m_WLAN.Initialize ())
	{
		return FALSE;
	}

	if (!m_Net.Initialize (FALSE))
	{
		return FALSE;
	}

	if (!m_WPASupplicant.Initialize ())
	{
		return FALSE;
	}

	m_bReportedNetwork = FALSE;
	m_pWebServer = 0;
	return TRUE;
}

void CWebServerService::Update (void)
{
	if (!m_Net.IsRunning ())
	{
		return;
	}

	if (!m_bReportedNetwork)
	{
		CString IPAddress;
		m_Net.GetConfig ()->GetIPAddress ()->Format (&IPAddress);
		CLogger::Get ()->Write (FromWebServerService, LogNotice,
				       "WLAN connected: %s",
				       static_cast<const char *> (IPAddress));
		m_bReportedNetwork = TRUE;
	}

	if (m_pWebServer != 0)
	{
		return;
	}

	m_pWebServer = new CSDWebServer (&m_Net, m_pWebRoot, m_pTimer, m_pScreen);
	assert (m_pWebServer != 0);

	CLogger::Get ()->Write (FromWebServerService, LogNotice,
				 "HTTP server listening on port 80 (root %s)", m_pWebRoot);
}

void CWebServerService::OnEvent (const Event &)
{
}

CString CWebServerService::GetIPAddress (void)
{
	CString IPAddress;
	m_Net.GetConfig ()->GetIPAddress ()->Format (&IPAddress);
	return IPAddress;
}
