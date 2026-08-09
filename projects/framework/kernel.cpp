#include "kernel.h"

#include <circle/memory.h>

CKernel::CKernel (void)
: CMultiCoreSupport (CMemorySystem::Get ()),
  m_Core0 (&m_EventRouter),
  m_Core1 (&m_EventRouter),
  m_Core2 (&m_EventRouter),
  m_Core3 (&m_EventRouter)
{
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	if (!m_Core0.Initialize ())
	{
		return FALSE;
	}

	// Start cores 1-3 only after core 0 has initialized all shared services.
	return CMultiCoreSupport::Initialize ();
}

TShutdownMode CKernel::Run (void)
{
	Run (0);

	return ShutdownHalt;
}

boolean CKernel::QueueEvent (unsigned nCore, const Event &EventToQueue)
{
	return m_EventRouter.QueueEvent (nCore, EventToQueue);
}

boolean CKernel::Subscribe (unsigned nCore, EventType Type)
{
	return m_EventRouter.Subscribe (nCore, Type);
}

boolean CKernel::Unsubscribe (unsigned nCore, EventType Type)
{
	return m_EventRouter.Unsubscribe (nCore, Type);
}

u32 CKernel::PublishEvent (const Event &EventToPublish)
{
	return m_EventRouter.Publish (EventToPublish.sourceCore, EventToPublish);
}

void CKernel::Run (unsigned nCore)
{
	switch (nCore)
	{
	case 0:
		m_Core0.Run ();
		break;

	case 1:
		m_Core1.Run ();
		break;

	case 2:
		m_Core2.Run ();
		break;

	case 3:
		m_Core3.Run ();
		break;
	}
}
