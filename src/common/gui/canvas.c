#include "canvas.h"

#include "amp.h"
#include "display/glcd.h"
#include "menu.h"
#include "rtc.h"
#include "swtimers.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit();

    canvas.glcd = glcdGet();
    canvas.layout = layoutGet();

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet(palIdx);

    glcdDrawRect(0, 0, dispdrv.width, dispdrv.height, canvas.pal->bg);

    canvas.glcd->rect = canvas.layout->rect;

    menuGet()->dispSize = 3/*canvas.layout->menu.itemCnt*/;
}

void canvasInit_()
{
    glcdInit();

    canvas.glcd = glcdGet();
    canvas.glcd->rect = canvas.layout->rect;

    canvas.layout = layoutGet();
    menuGet()->dispSize = canvas.layout->menu.itemCnt;

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet(palIdx);

    // Default font parameters
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(canvas.pal->fg);
    glcdSetFontBgColor(canvas.pal->bg);
}

Canvas *canvasGet(void)
{
    return &canvas;
}

void canvasClear(void)
{
    GlcdRect rect = canvas.glcd->rect;

    glcdDrawRect(0, 0, rect.w, rect.h, COLOR_BLACK);
    glcdShift(0);

    glcdSetFontColor(COLOR_WHITE);
    glcdSetFontBgColor(COLOR_BLACK);
}

void canvasShowTime(bool clear)
{
    (void)clear;

    const Layout *lt = layoutGet();
    const Palette *pal = paletteGet(paletteGetIndex());
    AmpStatus status = ampGet()->status;

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetXY(0, 0);
    glcdSetFont(lt->time.font);
    glcdSetFontColor(status == AMP_STATUS_STBY ? pal->inactive : pal->fg);
    glcdSetFontAlign(GLCD_ALIGN_LEFT);

    glcdWriteString(utilMkStr("%02d:%02d:%02d", rtc.hour, rtc.min, rtc.sec));
}

void canvasShowDate(bool clear)
{
    (void)clear;

    const Layout *lt = layoutGet();
    const Palette *pal = paletteGet(paletteGetIndex());
    AmpStatus status = ampGet()->status;

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetXY(0, 0);
    glcdSetFont(lt->time.font);
    glcdSetFontColor(status == AMP_STATUS_STBY ? pal->inactive : pal->fg);
    glcdSetFontAlign(GLCD_ALIGN_LEFT);

    glcdWriteString(utilMkStr("%02d.%02d.%04d", rtc.date, rtc.month, 2000 + rtc.year));
}

void canvasDebugFPS(void)
{
    const Layout *lt = layoutGet();
    const Palette *pal = paletteGet(paletteGetIndex());

    glcdSetFont(&fontterminus12);
    glcdSetFontColor(pal->inactive);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    glcdSetXY(lt->rect.w, 0);

    static int32_t oldCnt = 0;
    static int32_t oldFps = 0;
    static int32_t frames = 0;

    frames++;

    int32_t cnt = swTimGet(SW_TIM_SYSTEM);
    if (cnt - oldCnt > 500) {
        int32_t fps = frames * 1000 / (cnt - oldCnt);
        frames = 0;
        oldFps = fps;
        oldCnt = cnt;
    } else {
    }
    glcdWriteString(utilMkStr("%4d", oldFps));
}
