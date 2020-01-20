#include "layout.h"

static const Layout lt160x128 = {
    .rect.x = 0,
    .rect.y = 0,
    .rect.w = 256,
    .rect.h = 64,

    .time.hmsFont = &fontterminus20,
    .time.dmyFont = &fontterminus20,
    .time.wdFont = &fontterminus14,
    .time.hmsY = 4,
    .time.dmyY = 54,
    .time.wdY = 96,

    .menu.headFont = &fontterminus12,
    .menu.menuFont = &fontterminus12,
    .menu.itemCnt = 7,

    .tune.valFont = &fontterminus12,
    .tune.valY = 28,
    .tune.bar.sc = 58,
    .tune.bar.sw = 1,
    .tune.bar.barY = 30,
    .tune.bar.barW = 116,
    .tune.bar.half = 5,
    .tune.bar.middle = 2,

    .tuner.stFont = &fontterminus12,
    .tuner.nameFont = &fontterminus12,
    .tuner.bar.sc = 64,
    .tuner.bar.sw = 1,
    .tuner.bar.barY = 30,
    .tuner.bar.barW = 128,
    .tuner.bar.half = 5,
    .tuner.bar.middle = 2,
    .tuner.iconSpace = 1,

    .rds.psFont = &fontterminus12,
    .rds.textFont = &fontterminus12,

    .textEdit.rect.x = 2,
    .textEdit.rect.y = 29,
    .textEdit.rect.w = 156,
    .textEdit.rect.h = 70,
    .textEdit.editFont = &fontterminus12,

//    .iconSet = &iconsamp24,
    .lblFont = &fontterminus28b,
};

const Layout *layoutGet(void)
{
    return &lt160x128;
}
