#ifndef LAYOUT_H
#define LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "display/glcd.h"
//#include "icons.h"
#include "widget/spectrumcolumn.h"
#include "widget/stripedbar.h"
#include "widget/textedit.h"

typedef struct {
    const tFont *hmsFont;       // Font to draw hours/minutes/seconds
    const tFont *dmyFont;       // Font to draw day/month/year
    const tFont *wdFont;        // Font to draw weekday
    uint8_t hmsY;               // Y position of hours/minutes/seconds
    uint8_t dmyY;               // Y position of day/month/year
    uint8_t wdY;                // Y position of weekday
} LayoutTime;

typedef struct {
    const tFont *headFont;      // Font to draw menu header
    const tFont *menuFont;      // Foft to draw menu item
    uint8_t itemCnt;            // Number of items can be shown
} LayoutMenu;

typedef struct {
    const tFont *valFont;       // Foft to draw tune label
    int16_t valY;               // Y position of the tune value
    LayoutStripedBar bar;
} LayoutTune;

typedef struct {
    GlcdRect rect;

    LayoutTime time;
    LayoutMenu menu;
    LayoutTune tune;

    const tFont *lblFont;       // Main label font
    const tFont *iconSet;       // Main icon set
} Layout;

const Layout *layoutGet(void);

#ifdef __cplusplus
}
#endif

#endif // LAYOUT_H
