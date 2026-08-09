#include "storageservice.h"

#include <circle/logger.h>
#include <assert.h>

static const char FromStorageService[] = "storage";

CStorageService::CStorageService (CInterruptSystem *pInterruptSystem, CTimer *pTimer,
				  CActLED *pActLED, const char *pDrive)
: CService (PeriodDisabled),
  m_pDrive (pDrive),
  m_EMMC (pInterruptSystem, pTimer, pActLED)
{
	assert (pInterruptSystem != 0);
	assert (pTimer != 0);
	assert (pActLED != 0);
	assert (m_pDrive != 0);
}

CStorageService::~CStorageService (void)
{
	m_pDrive = 0;
}

boolean CStorageService::InitService (void)
{
	if (!m_EMMC.Initialize ())
	{
		return FALSE;
	}

	if (f_mount (&m_FileSystem, m_pDrive, 1) != FR_OK)
	{
		CLogger::Get ()->Write (FromStorageService, LogError,
				 "Cannot mount %s", m_pDrive);
		return FALSE;
	}

	return TRUE;
}

void CStorageService::Update (void)
{
}

void CStorageService::OnEvent (const Event &)
{
}

const char *CStorageService::GetDrive (void) const
{
	return m_pDrive;
}
