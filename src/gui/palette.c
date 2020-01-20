#include "palette.h"

static PalIdx palIdx = PAL_DEFAULT;

static const Palette palette[PAL_END] = {
    [PAL_DEFAULT] = {
        .fg = LCD_COLOR_WHITE,
        .bg = LCD_COLOR_BLACK,
        .active = LCD_COLOR_WHITE,
        .inactive = RGB_CONV(0x222222),
        .selected =  RGB_CONV(0x444444),
        .spColB = LCD_COLOR_WHITE,
        .spColG = LCD_COLOR_WHITE,
        .spPeak = LCD_COLOR_WHITE,
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
