#pragma once

#include "networkservice.h"
#include "service.h"
#include "webserver.h"

#include <circle/screen.h>
#include <circle/timer.h>

class CWebServerService : public CService
{
public:
	CWebServerService (CNetworkService *pNetworkService, CTimer *pTimer,
			   CScreenDevice *pScreen, const char *pWebRoot);
	~CWebServerService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	CNetworkService *m_pNetworkService;
	CTimer *m_pTimer;
	CScreenDevice *m_pScreen;
	const char *m_pWebRoot;
	CSDWebServer *m_pWebServer;
};
