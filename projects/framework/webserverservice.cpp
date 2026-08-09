#include "webserverservice.h"

#include <circle/logger.h>
#include <assert.h>

static const char FromWebServerService[] = "webserver";

CWebServerService::CWebServerService (CNetworkService *pNetworkService,
				      CTimer *pTimer, CScreenDevice *pScreen,
				      const char *pWebRoot)
: CService (PeriodPolling),
  m_pNetworkService (pNetworkService),
  m_pTimer (pTimer),
  m_pScreen (pScreen),
  m_pWebRoot (pWebRoot),
  m_pWebServer (0)
{
	assert (m_pNetworkService != 0);
	assert (m_pTimer != 0);
	assert (m_pScreen != 0);
	assert (m_pWebRoot != 0);
}

CWebServerService::~CWebServerService (void)
{
	// CSDWebServer is a process-lifetime scheduler task. It is intentionally
	// not deleted while the scheduler may still run it.
	m_pWebServer = 0;
	m_pWebRoot = 0;
	m_pScreen = 0;
	m_pTimer = 0;
	m_pNetworkService = 0;
}

boolean CWebServerService::InitService (void)
{
	m_pWebServer = 0;
	return TRUE;
}

void CWebServerService::Update (void)
{
	if (m_pWebServer != 0 || !m_pNetworkService->IsRunning ())
	{
		return;
	}

	m_pWebServer = new CSDWebServer (m_pNetworkService->GetNetSubSystem (),
					m_pWebRoot, m_pTimer, m_pScreen);
	assert (m_pWebServer != 0);

	CLogger::Get ()->Write (FromWebServerService, LogNotice,
				 "HTTP server listening on port 80 (root %s)", m_pWebRoot);
}

void CWebServerService::OnEvent (const Event &)
{
}
