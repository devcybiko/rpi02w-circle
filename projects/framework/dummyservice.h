#pragma once

#include "service.h"

class CDummyService : public CService
{
public:
	CDummyService (void);
	~CDummyService (void) override;

	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;
};
