#include "dummyservice.h"

#include <circle/logger.h>

CDummyService::CDummyService (void)
: CService (PeriodDisabled)
{
}

CDummyService::~CDummyService (void)
{
}

void CDummyService::Update (void)
{
}

void CDummyService::OnEvent (const Event &EventToHandle)
{
	CLogger::Get ()->Write ("dummy_service", LogNotice,
			       "Event type %u from core %u",
			       static_cast<unsigned> (EventToHandle.type),
			       static_cast<unsigned> (EventToHandle.sourceCore));
}
