#pragma once

#include "service.h"

#include <circle/serial.h>

class CSerialService : public CService
{
public:
	explicit CSerialService (unsigned nBaudRate = 115200);
	~CSerialService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

	CSerialDevice *GetSerial (void);

private:
	unsigned m_nBaudRate;
	CSerialDevice m_Serial;
};
