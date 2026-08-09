#pragma once

#include "service.h"

#include <SDCard/emmc.h>
#include <circle/actled.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <fatfs/ff.h>

class CStorageService : public CService
{
public:
	CStorageService (CInterruptSystem *pInterruptSystem, CTimer *pTimer,
			 CActLED *pActLED, const char *pDrive);
	~CStorageService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

	const char *GetDrive (void) const;

private:
	const char *m_pDrive;
	CEMMCDevice m_EMMC;
	FATFS m_FileSystem;
};
