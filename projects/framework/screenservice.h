#pragma once

#include "service.h"

#include <circle/2dgraphics.h>
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
	C2DGraphics *GetGraphics (void);

private:
	CScreenDevice m_Screen;
	C2DGraphics *m_pGraphics;
};
