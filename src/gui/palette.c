#include "palette.h"

static PalIdx palIdx = PAL_DEFAULT;

static const Palette palette[PAL_END] = {
    [PAL_DEFAULT] = {
        .fg = COLOR_WHITE,
        .bg = COLOR_BLACK,
        .active = COLOR_WHITE,
        .inactive = COLOR_GRAY16(6),
        .selected =  COLOR_GRAY16(4),
        .spColB = COLOR_WHITE,
        .spColG = COLOR_WHITE,
        .spPeak = COLOR_WHITE,
    },
};

const Palette *paletteGet(PalIdx index)
{
    return &palette[index];
}

PalIdx paletteGetIndex(void)
{
    return palIdx;
}

void paletteSetIndex(PalIdx index)
{
    palIdx = index;
}
