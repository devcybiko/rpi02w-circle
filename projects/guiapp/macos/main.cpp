#include "guiapp.h"

#include <src/drivers/sdl/lv_sdl_window.h>

int main (void)
{
    lv_init ();

    lv_display_t *pDisplay = lv_sdl_window_create (800, 480);
    if (pDisplay == 0)
    {
        return 1;
    }

    lv_sdl_window_set_title (pDisplay, "LVGL Simulator");
    GuiAppCreate ("Running on macOS");

    for (;;)
    {
        lv_timer_handler ();
        lv_delay_ms (5);
    }
}
