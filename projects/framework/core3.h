#ifndef _framework_core3_h
#define _framework_core3_h

#include "core.h"

class CCore3 : public CCore
{
public:
	CCore3 (void);

protected:
	void HandleEvent (const Event &EventToHandle) override;
};

#endif
