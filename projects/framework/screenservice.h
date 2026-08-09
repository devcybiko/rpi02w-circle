#pragma once

#include "service.h"

#include <circle/screen.h>

class CScreenService : public CService
{
public:
	CScreenService (unsigned nWidth, unsigned nHeight);
	~CScreenService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

	CScreenDevice *GetScreen (void);

private:
	CScreenDevice m_Screen;
};
