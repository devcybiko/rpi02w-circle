#ifndef _framework_kernel_h
#define _framework_kernel_h

#include "core0.h"
#include "core1.h"
#include "core2.h"
#include "core3.h"
#include "eventrouter.h"

#include <circle/multicore.h>
#include <circle/types.h>

enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};

class CKernel : public CMultiCoreSupport
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);
	TShutdownMode Run (void);
	boolean QueueEvent (unsigned nCore, const Event &EventToQueue);
	boolean Subscribe (unsigned nCore, EventType Type);
	boolean Unsubscribe (unsigned nCore, EventType Type);
	u32 PublishEvent (const Event &EventToPublish);

protected:
	void Run (unsigned nCore) override;

private:
	CEventRouter m_EventRouter;
	CCore0 m_Core0;
	CCore1 m_Core1;
	CCore2 m_Core2;
	CCore3 m_Core3;
};

#endif
