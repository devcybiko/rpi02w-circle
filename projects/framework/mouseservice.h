#pragma once

#include "service.h"

#include <circle/devicenameservice.h>
#include <circle/input/mouse.h>
#include <circle/screen.h>

class CMouseService : public CService
{
public:
	CMouseService (CDeviceNameService *pDeviceNameService, CScreenDevice *pScreen);
	~CMouseService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	static void MouseRemovedHandler (CDevice *pDevice, void *pContext);
	static void MouseEventHandler (TMouseEvent Event, unsigned nButtons,
				       unsigned nPosX, unsigned nPosY, int nWheelMove);

private:
	CDeviceNameService *m_pDeviceNameService;
	CScreenDevice *m_pScreen;
	CMouseDevice * volatile m_pMouse;

	static CMouseService *s_pThis;
};
