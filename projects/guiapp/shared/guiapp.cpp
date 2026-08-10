#include "guiapp.h"

#include <lvgl.h>

namespace
{
lv_obj_t *s_pStatusLabel = 0;
}

void GuiAppCreate (const char *pStatus)
{
    lv_obj_t *pScreen = lv_screen_active ();
    if (pScreen == 0)
    {
        return;
    }

    lv_obj_set_style_bg_color (pScreen, lv_color_hex (0x202020), LV_PART_MAIN);

    lv_obj_t *pPanel = lv_obj_create (pScreen);
    if (pPanel == 0)
    {
        return;
    }

    lv_obj_set_size (pPanel, 320, 160);
    lv_obj_center (pPanel);
    lv_obj_set_flex_flow (pPanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align (pPanel, LV_FLEX_ALIGN_CENTER,
                            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *pTitle = lv_label_create (pPanel);
    if (pTitle == 0)
    {
        return;
    }
    lv_label_set_text (pTitle, "Raspberry Pi Zero 2 W");

    s_pStatusLabel = lv_label_create (pPanel);
    GuiAppSetStatus (pStatus);
}

void GuiAppSetStatus (const char *pStatus)
{
    if (s_pStatusLabel != 0)
    {
        lv_label_set_text (s_pStatusLabel, pStatus != 0 ? pStatus : "");
    }
}
