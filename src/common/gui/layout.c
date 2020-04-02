#include "layout.h"

static const Layout lt256x64 = {
    .rect.x = 0,
    .rect.y = 0,
    .rect.w = 256,
    .rect.h = 64,

    .time.font = &fontterminusdig64,
    .time.posY = 0,

    .date.font = &fontterminusdig40,
    .date.posY = 12,

    .dTime.hmsFont = &fontterminus24b,
    .dTime.dmyFont = &fontterminus24b,
    .dTime.wdFont = &fontterminus16b,
    .dTime.hmsY = 0,
    .dTime.dmyY = 20,
    .dTime.wdY = 40,

    .menu.headFont = &fontterminus12,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 3,

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
    return &lt256x64;
}
