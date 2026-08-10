#include "core1.h"
#include "dummyservice.h"

#include <assert.h>

CCore1::CCore1 (CEventRouter *pEventRouter)
: CCore (pEventRouter, 1, TRUE)
{
	boolean bOK = TRUE;
	// bOK = m_Dispatcher.AddService (new CDummyService);

	for (unsigned nType = static_cast<unsigned> (EventType::None) + 1;
	     bOK && nType < static_cast<unsigned> (EventType::Count); nType++)
	{
		bOK = Subscribe (static_cast<EventType> (nType));
	}

	assert (bOK);
}

CCore1::~CCore1 (void)
{
	m_Dispatcher.RemoveServices ();
}
