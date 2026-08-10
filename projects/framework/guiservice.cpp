#include "guiservice.h"
#include "core.h"
#include "webserverservice.h"

#include <circle/logger.h>
#include <assert.h>
#include <string.h>
#include <circle/timer.h>

//
// LVGL partial-render buffer.
//
// For now support displays up to 1920 pixels wide and give LVGL
// 20 scanlines of RGB565 working space.
//
// The explicit LVGL alignment attribute avoids relying on the
// alignment provided by C++ new[].
//
static const unsigned LVGLDrawBufferLines = 20;
static const unsigned LVGLMaxScreenWidth = 1920;
static bool s_bLVGLDrawBufferInitialized = false;

LV_ATTRIBUTE_MEM_ALIGN
static uint16_t s_LVGLDrawBuffer[
    LVGLMaxScreenWidth * LVGLDrawBufferLines
];


CGuiService::CGuiService (CScreenService *pScreenService)
: CService (5),
  m_pScreenService (pScreenService),
  m_pDisplay (0),
  m_DrawBuffer {},
  m_nMouseX (0),
  m_nMouseY (0),
  m_bMousePressed (FALSE),
m_pMouse (0)
{
    assert (m_pScreenService != 0);
}


CGuiService::~CGuiService (void)
{
    m_pDisplay = 0;
    m_pScreenService = 0;
}


boolean CGuiService::InitService (void)
{
    CScreenDevice *pScreen =
        m_pScreenService->GetScreen ();

    C2DGraphics *pGraphics =
        m_pScreenService->GetGraphics ();

    if (pScreen == 0 || pGraphics == 0)
    {
        return FALSE;
    }

    const unsigned nWidth =
        pGraphics->GetWidth ();

    const unsigned nHeight =
        pGraphics->GetHeight ();

    if (nWidth == 0 || nHeight == 0)
    {
        return FALSE;
    }

    if (nWidth > LVGLMaxScreenWidth)
    {
        CLogger::Get ()->Write (
            "guiservice.cpp",
            LogError,
            "Screen width %u exceeds LVGL maximum %u",
            nWidth,
            LVGLMaxScreenWidth);

        return FALSE;
    }

    CLogger::Get ()->Write (
        "guiservice.cpp",
        LogNotice,
        "LVGL init");

    //
    // Initialize LVGL.
    //
    lv_init ();

    lv_tick_set_cb (GetTick);

    //
    // Create the LVGL display.
    //
    CLogger::Get ()->Write (
        "guiservice.cpp",
        LogNotice,
        "LVGL creating display %ux%u",
        nWidth,
        nHeight);

    m_pDisplay =
        lv_display_create (nWidth, nHeight);

    if (m_pDisplay == 0)
    {
        CLogger::Get ()->Write (
            "guiservice.cpp",
            LogError,
            "lv_display_create failed");

        return FALSE;
    }

    lv_display_set_user_data (
        m_pDisplay,
        this);

    lv_display_set_flush_cb (
        m_pDisplay,
        FlushDisplay);

    //
    // LVGL renders internally as RGB565.
    //
    lv_display_set_color_format (
        m_pDisplay,
        LV_COLOR_FORMAT_RGB565);

    //
    // Configure the LVGL draw buffer.
    //
    const lv_color_format_t cf =
        lv_display_get_color_format (
            m_pDisplay);

    const uint32_t w =
        lv_display_get_original_horizontal_resolution (
            m_pDisplay);

    const uint32_t stride =
        lv_draw_buf_width_to_stride (
            w,
            cf);

    const unsigned nDrawBufferBytes =
        w
        * LVGLDrawBufferLines
        * sizeof (uint16_t);

    lv_draw_buf_init (
        &m_DrawBuffer,
        w,
        LVGLDrawBufferLines,
        cf,
        stride,
        s_LVGLDrawBuffer,
        nDrawBufferBytes);

    lv_display_set_draw_buffers (
        m_pDisplay,
        &m_DrawBuffer,
        0);

    lv_display_set_render_mode (
        m_pDisplay,
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    //
    // Create an LVGL mouse/pointer input device.
    //
    m_pMouse =
        lv_indev_create ();

    if (m_pMouse == 0)
    {
        CLogger::Get ()->Write (
            "guiservice.cpp",
            LogError,
            "lv_indev_create failed");

        return FALSE;
    }

    lv_indev_set_type (
        m_pMouse,
        LV_INDEV_TYPE_POINTER);

    lv_indev_set_user_data (
        m_pMouse,
        this);

    lv_indev_set_read_cb (
        m_pMouse,
        ReadMouse);

    //
    // Associate the mouse with this display.
    //
    lv_indev_set_display (
        m_pMouse,
        m_pDisplay);

    //
    // Give the screen a visible background.
    //
    lv_obj_t *pLVScreen =
        lv_screen_active ();

    if (pLVScreen == 0)
    {
        return FALSE;
    }

    lv_obj_set_style_bg_color (
        pLVScreen,
        lv_color_hex (0x202020),
        LV_PART_MAIN);

    return TRUE;
}

void CGuiService::ReadMouse (
    lv_indev_t *pIndev,
    lv_indev_data_t *pData)
{
    CGuiService *pThis =
        static_cast<CGuiService *> (
            lv_indev_get_user_data (pIndev));

    assert (pThis != 0);
    assert (pData != 0);

    pData->point.x =
        pThis->m_nMouseX;

    pData->point.y =
        pThis->m_nMouseY;

    pData->state =
        pThis->m_bMousePressed
            ? LV_INDEV_STATE_PRESSED
            : LV_INDEV_STATE_RELEASED;
}

uint32_t CGuiService::GetTick (void)
{
    return CTimer::Get ()->GetTicks () * 1000 / HZ;
}

void CGuiService::Update (void)
{
    lv_timer_handler ();
    CWebServerService *webService = static_cast<CWebServerService *>(pCore0->FindService ("webserver"));
    CString IPAddress = webService->GetIPAddress ();
    if (IPAddress.Compare("0.0.0.0") == 0 || s_bLVGLDrawBufferInitialized)
    {
        return;
    }
    //
    // Create a button in the center of the screen.
    //
    lv_obj_t *pButton =
        lv_button_create (lv_screen_active ());

    if (pButton == 0)
    {
        return;
    }

    lv_obj_set_size (
        pButton,
        240,
        80);

    lv_obj_center (
        pButton);

    //
    // Put a label on the button.
    //
    lv_obj_t *pLabel =
        lv_label_create (pButton);

    if (pLabel == 0)
    {
        return;
    }


    lv_label_set_text (
        pLabel,
        IPAddress.c_str ());

    lv_obj_center (
        pLabel);

        s_bLVGLDrawBufferInitialized = true;

}

void CGuiService::OnEvent (const Event &EventToHandle)
{
    //
    // Store the current mouse state.
    //
    // We aren't exposing this to LVGL yet, but the service is
    // already ready for the input-device integration.
    //
    if (EventToHandle.type == EventType::Mouse)
    {
        m_nMouseX =
            EventToHandle.mouse.x;

        m_nMouseY =
            EventToHandle.mouse.y;

        m_bMousePressed =
            EventToHandle.mouse.buttons != 0;
    }
}


void CGuiService::FlushDisplay (lv_display_t *pDisplay,
                                const lv_area_t *pArea,
                                uint8_t *pPixelMap)
{
    assert (pDisplay != 0);
    assert (pArea != 0);
    assert (pPixelMap != 0);

    CGuiService *pThis =
        static_cast<CGuiService *> (
            lv_display_get_user_data (pDisplay));

    assert (pThis != 0);
    assert (pThis->m_pScreenService != 0);

    CScreenDevice *pScreen =
        pThis->m_pScreenService->GetScreen ();

    assert (pScreen != 0);

    CBcmFrameBuffer *pFrameBuffer =
        pScreen->GetFrameBuffer ();

    assert (pFrameBuffer != 0);

    uint8_t *pFrameBufferPixels =
        reinterpret_cast<uint8_t *> (
            static_cast<uintptr> (pFrameBuffer->GetBuffer ()));

    uint16_t *pSource =
        reinterpret_cast<uint16_t *> (
            pPixelMap);

    assert (pFrameBufferPixels != 0);

    const unsigned nFrameBufferPitch =
        pFrameBuffer->GetPitch ();

    const unsigned nAreaWidth =
        static_cast<unsigned> (
            pArea->x2 - pArea->x1 + 1);

    const unsigned nAreaHeight =
        static_cast<unsigned> (
            pArea->y2 - pArea->y1 + 1);

    //
    // Copy LVGL's rendered rectangle into the Circle framebuffer.
    //
    // This currently assumes both buffers contain compatible
    // 16-bit pixels. We'll handle RGB565 -> RGB555 properly later.
    //
    for (unsigned y = 0;
         y < nAreaHeight;
         ++y)
    {
        uint8_t *pDestination =
            pFrameBufferPixels
            + (static_cast<unsigned> (pArea->y1) + y)
              * nFrameBufferPitch
            + static_cast<unsigned> (pArea->x1) * sizeof (uint16_t);

        memcpy (
            pDestination,
            pSource + y * nAreaWidth,
            nAreaWidth * sizeof (uint16_t));
    }

    //
    // Release the LVGL render buffer.
    //
    lv_display_flush_ready (pDisplay);
}
