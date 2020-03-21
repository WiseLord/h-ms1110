#include "layout.h"

static const Layout lt160x128 = {
    .rect.x = 0,
    .rect.y = 0,
    .rect.w = 256,
    .rect.h = 64,

    .time.hmsFont = &fontterminus24b,
    .time.dmyFont = &fontterminus24b,
    .time.wdFont = &fontterminus16b,
    .time.hmsY = 0,
    .time.dmyY = 20,
    .time.wdY = 40,

    .menu.headFont = &fontterminus12,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminus32,
    .tune.valY = 0,
    .tune.bar.sc = 64,
    .tune.bar.sw = 2,
    .tune.bar.barY = 42,
    .tune.bar.barW = 256,
    .tune.bar.half = 5,
    .tune.bar.middle = 2,

//    .iconSet = &iconsamp24,
    .lblFont = &fontterminus28b,
};

const Layout *layoutGet(void)
{
    return &lt160x128;
}
