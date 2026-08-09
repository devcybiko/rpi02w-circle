#ifndef _framework_core1_h
#define _framework_core1_h

#include "core.h"

class CCore1 : public CCore
{
public:
	CCore1 (void);

protected:
	void HandleEvent (const Event &EventToHandle) override;
};

#endif
