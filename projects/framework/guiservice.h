#pragma once

#include "service.h"
#include "screenservice.h"

#include <lvgl.h>

class CGuiService : public CService
{
public:
    CGuiService (CScreenService *pScreenService);
    ~CGuiService (void) override;

    boolean InitService (void) override;
    void Update (void) override;
    void OnEvent (const Event &EventToHandle) override;

private:
    static uint32_t GetTick (void);

    static void FlushDisplay (lv_display_t *pDisplay,
                              const lv_area_t *pArea,
                              uint8_t *pPixelMap);
static void ReadMouse (lv_indev_t *pIndev,
                       lv_indev_data_t *pData);
private:
    CScreenService *m_pScreenService;
    lv_display_t *m_pDisplay;
    lv_draw_buf_t m_DrawBuffer;

    int m_nMouseX;
    int m_nMouseY;
    boolean m_bMousePressed;
    lv_indev_t *m_pMouse;
};
