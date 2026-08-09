#pragma once

#include "service.h"

#include <wlan/bcm4343.h>

class CWLANService : public CService
{
public:
	explicit CWLANService (const char *pFirmwarePath);
	~CWLANService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	const char *m_pFirmwarePath;
	CBcm4343Device m_WLAN;
};
