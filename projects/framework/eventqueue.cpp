#include "eventqueue.h"

CEventQueue::CEventQueue (void)
: m_nRead (0),
  m_nWrite (0),
  m_nCount (0)
{
}

boolean CEventQueue::Push (const Event &EventToPush)
{
	m_Lock.Acquire ();

	if (m_nCount == Capacity)
	{
		m_Lock.Release ();
		return FALSE;
	}

	m_Events[m_nWrite] = EventToPush;
	m_nWrite = (m_nWrite + 1) % Capacity;
	m_nCount++;

	m_Lock.Release ();
	return TRUE;
}

boolean CEventQueue::Pop (Event &EventToPop)
{
	m_Lock.Acquire ();

	if (m_nCount == 0)
	{
		m_Lock.Release ();
		return FALSE;
	}

	EventToPop = m_Events[m_nRead];
	m_nRead = (m_nRead + 1) % Capacity;
	m_nCount--;

	m_Lock.Release ();
	return TRUE;
}
