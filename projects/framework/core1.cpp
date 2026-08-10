#include "core1.h"
#include "dummyservice.h"

#include <assert.h>

CCore1::CCore1 (CEventRouter *pEventRouter)
: CCore (pEventRouter, 1, TRUE)
{
}

CCore1::~CCore1 (void)
{
	m_Dispatcher.RemoveServices ();
}
