#include "kernel.h"

#include <circle/memory.h>

CKernel::CKernel (void)
: CMultiCoreSupport (CMemorySystem::Get ())
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
	switch (nCore)
	{
	case 0:
		return m_Core0.QueueEvent (EventToQueue);

	case 1:
		return m_Core1.QueueEvent (EventToQueue);

	case 2:
		return m_Core2.QueueEvent (EventToQueue);

	case 3:
		return m_Core3.QueueEvent (EventToQueue);

	default:
		return FALSE;
	}
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
