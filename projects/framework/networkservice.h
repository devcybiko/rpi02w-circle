#pragma once

#include "service.h"

#include <circle/net/netsubsystem.h>
#include <circle/types.h>

class CNetworkService : public CService
{
public:
	CNetworkService (void);
	~CNetworkService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

	boolean IsRunning (void) const;
	CNetSubSystem *GetNetSubSystem (void);

private:
	CNetSubSystem m_Net;
	boolean m_bReportedNetwork;
};
