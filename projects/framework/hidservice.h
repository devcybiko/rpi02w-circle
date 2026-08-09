#pragma once

#include "service.h"

#include <circle/devicenameservice.h>
#include <circle/input/mouse.h>
#include <circle/interrupt.h>
#include <circle/screen.h>
#include <circle/timer.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbkeyboard.h>

class CHIDService : public CService
{
public:
	CHIDService (CInterruptSystem *pInterruptSystem, CTimer *pTimer,
		     CDeviceNameService *pDeviceNameService, CScreenDevice *pScreen);
	~CHIDService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	void AttachKeyboard (void);
	void AttachMouse (void);

	static void KeyPressedHandler (const char *pString);
	static void KeyboardRemovedHandler (CDevice *pDevice, void *pContext);
	static void MouseRemovedHandler (CDevice *pDevice, void *pContext);
	static void MouseEventHandler (TMouseEvent Event, unsigned nButtons,
				       unsigned nPosX, unsigned nPosY, int nWheelMove);
	static boolean LogDevice (CDevice *pDevice, const char *pName,
				  boolean bBlockDevice, void *pContext);

private:
	CDeviceNameService *m_pDeviceNameService;
	CScreenDevice *m_pScreen;
	CUSBHCIDevice m_USBHCI;
	CUSBKeyboardDevice * volatile m_pKeyboard;
	CMouseDevice * volatile m_pMouse;

	static CHIDService *s_pThis;
};
