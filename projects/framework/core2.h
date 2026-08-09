#ifndef _framework_core2_h
#define _framework_core2_h

#include "core.h"

class CCore2 : public CCore
{
public:
	CCore2 (void);

protected:
	void HandleEvent (const Event &EventToHandle) override;
};

#endif
