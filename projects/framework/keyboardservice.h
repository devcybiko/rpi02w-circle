#pragma once

#include "service.h"

#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/usb/usbkeyboard.h>

class CKeyboardService : public CService
{
public:
	CKeyboardService (CDeviceNameService *pDeviceNameService, CScreenDevice *pScreen);
	~CKeyboardService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	static void KeyPressedHandler (const char *pString);
	static void KeyboardRemovedHandler (CDevice *pDevice, void *pContext);

private:
	CDeviceNameService *m_pDeviceNameService;
	CScreenDevice *m_pScreen;
	CUSBKeyboardDevice * volatile m_pKeyboard;

	static CKeyboardService *s_pThis;
};
