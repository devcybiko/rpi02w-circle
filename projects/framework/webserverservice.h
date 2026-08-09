#pragma once

#include "service.h"
#include "webserver.h"

#include <circle/net/netsubsystem.h>
#include <circle/screen.h>
#include <circle/timer.h>
#include <wlan/bcm4343.h>
#include <wlan/hostap/wpa_supplicant/wpasupplicant.h>

class CWebServerService : public CService
{
public:
	CWebServerService (const char *pFirmwarePath, const char *pConfigFile,
			   CTimer *pTimer, CScreenDevice *pScreen,
			   const char *pWebRoot);
	~CWebServerService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	const char *m_pFirmwarePath;
	const char *m_pConfigFile;
	CTimer *m_pTimer;
	CScreenDevice *m_pScreen;
	const char *m_pWebRoot;
	CBcm4343Device m_WLAN;
	CNetSubSystem m_Net;
	CWPASupplicant m_WPASupplicant;
	boolean m_bReportedNetwork;
	CSDWebServer *m_pWebServer;
};
