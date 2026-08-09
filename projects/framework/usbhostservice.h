#pragma once

#include "service.h"

#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/usb/usbhcidevice.h>

class CUSBHostService : public CService
{
public:
	CUSBHostService (CInterruptSystem *pInterruptSystem, CTimer *pTimer);
	~CUSBHostService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	CUSBHCIDevice m_USBHCI;
};
