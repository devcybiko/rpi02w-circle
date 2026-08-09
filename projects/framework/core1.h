#ifndef _framework_core1_h
#define _framework_core1_h

#include "core.h"

class CCore1 : public CCore
{
public:
	explicit CCore1 (CEventRouter *pEventRouter);

protected:
	void HandleEvent (const Event &EventToHandle) override;
};

#endif
