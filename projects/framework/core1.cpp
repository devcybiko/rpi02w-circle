#include "core1.h"

CCore1::CCore1 (CEventRouter *pEventRouter)
: CCore (pEventRouter, 1, TRUE)
{
}

void CCore1::HandleEvent (const Event &)
{
}
