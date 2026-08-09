#include "serialservice.h"

CSerialService::CSerialService (unsigned nBaudRate)
: CService (PeriodDisabled),
  m_nBaudRate (nBaudRate)
{
}

CSerialService::~CSerialService (void)
{
}

boolean CSerialService::InitService (void)
{
	return m_Serial.Initialize (m_nBaudRate);
}

void CSerialService::Update (void)
{
}

void CSerialService::OnEvent (const Event &)
{
}

CSerialDevice *CSerialService::GetSerial (void)
{
	return &m_Serial;
}
