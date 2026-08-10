#pragma once

#include "screenservice.h"

class CDrawService : public CService
{
public:
	explicit CDrawService (CScreenService *pScreenService);
	~CDrawService (void) override;

	boolean InitService (void) override;
	void Update (void) override;
	void OnEvent (const Event &EventToHandle) override;

private:
	boolean m_bDrawing;
	int m_nLastX;
	int m_nLastY;
	CScreenService *m_pScreenService;
	C2DGraphics *m_pGraphics;
};
