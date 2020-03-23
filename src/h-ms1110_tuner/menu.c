#include "menu.h"

#include <string.h>

#include "amp.h"
#include "input.h"
#include "rc.h"
#include "screen.h"
#include "settings.h"
#include "tr/labels.h"
#include "utils.h"

#define GENERATE_MENU_ITEM(CMD)    [MENU_RC_ ## CMD] = {MENU_SETUP_RC, MENU_TYPE_RC, PARAM_RC_ ## CMD},

static Menu menu;

typedef struct {
    MenuIdx parent;
    MenuType type;
    Param param;
} MenuItem;

static const MenuItem menuItems[MENU_END] = {
//   menu index                parent menu              menu type           parameter
    [MENU_NULL]             = {MENU_NULL,               MENU_TYPE_PARENT,   PARAM_NULL},

    [MENU_SETUP]            = {MENU_NULL,               MENU_TYPE_PARENT,   PARAM_NULL},

    [MENU_SETUP_SYSTEM]     = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SETUP_TUNER]      = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},

    [MENU_SYSTEM_ENC_RES]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_ENC_RES},
    [MENU_SYSTEM_RTC_CORR]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_RTC_CORR},

    [MENU_TUNER_IC]         = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_IC},
    [MENU_TUNER_BAND]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_BAND},
    [MENU_TUNER_STEP]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_STEP},
    [MENU_TUNER_DEEMPH]     = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_DEEMPH},
    [MENU_TUNER_STA_MODE]   = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_STA_MODE},
    [MENU_TUNER_FMONO]      = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_FMONO},
    [MENU_TUNER_RDS]        = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_RDS},
    [MENU_TUNER_BASS]       = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_BASS},
    [MENU_TUNER_VOLUME]     = {MENU_SETUP_TUNER,        MENU_TYPE_NUMBER,   PARAM_TUNER_VOLUME},
};

static void menuMove(int8_t diff)
{
    int8_t newIdx = 0;

    for (int8_t idx = 0; idx < MENU_MAX_LEN; idx++) {
        if (menu.list[idx] == menu.active) {
            newIdx = idx;
            break;
        }
    }
    newIdx = (int8_t)((newIdx + menu.listSize + diff) % menu.listSize);

    menu.active = menu.list[newIdx];

    // Recalculate offset if needed
    if (menu.dispOft < newIdx - (menu.dispSize - 1)) {
        menu.dispOft = newIdx - (menu.dispSize - 1);
    } else if (menu.dispOft > newIdx) {
        menu.dispOft = newIdx;
    }
}

static int16_t menuGetValue(MenuIdx index)
{
    int16_t ret = 0;

    ret = settingsGet(menuItems[index].param);

    return ret;
}

static void menuStoreCurrentValue(void)
{
    if (menu.active < MENU_END) {
        Param param = menuItems[menu.active].param;
        settingsSet(param, menu.value);
        settingsStore(param, menu.value);
    }

    switch (menu.active) {
    case MENU_SYSTEM_RTC_CORR:
        rtcSetCorrection(menu.value);
        break;
    default:
        break;
    }
}

static void menuValueChange(int8_t diff)
{
    if (menuItems[menu.active].type == MENU_TYPE_BOOL) {
        if (diff)
            menu.value = !menu.value;
        return;
    }

    if (menuItems[menu.active].type == MENU_TYPE_RC) {
        RcData rcData = rcRead(false);

        menu.value = (int16_t)(((rcData.addr & 0xFF) << 8) | rcData.cmd);
        return;
    }

    menu.value += diff;

    switch (menu.active) {
    case MENU_SYSTEM_ENC_RES:
        if (menu.value > ENC_RES_MAX)
            menu.value = ENC_RES_MAX;
        if (menu.value < ENC_RES_MIN)
            menu.value = ENC_RES_MIN;
        break;
    case MENU_SYSTEM_RTC_CORR:
        if (menu.value > 64)
            menu.value = 64;
        if (menu.value < -63)
            menu.value = -63;
        break;

    case MENU_TUNER_IC:
        if (menu.value > TUNER_IC_END - 1)
            menu.value = TUNER_IC_END - 1;
        if (menu.value < TUNER_IC_NO)
            menu.value = TUNER_IC_NO;
        break;
    case MENU_TUNER_BAND:
        if (menu.value > TUNER_BAND_END - 1)
            menu.value = TUNER_BAND_END - 1;
        if (menu.value < TUNER_BAND_FM_US_EUROPE)
            menu.value = TUNER_BAND_FM_US_EUROPE;
        break;
    case MENU_TUNER_STEP:
        if (menu.value > TUNER_STEP_END - 1)
            menu.value = TUNER_STEP_END - 1;
        if (menu.value < TUNER_STEP_50K)
            menu.value = TUNER_STEP_50K;
        break;
    case MENU_TUNER_DEEMPH:
        if (menu.value > TUNER_DEEMPH_END - 1)
            menu.value = TUNER_DEEMPH_END - 1;
        if (menu.value < TUNER_DEEMPH_50u)
            menu.value = TUNER_DEEMPH_50u;
        break;
    case MENU_TUNER_VOLUME:
        if (menu.value > TUNER_VOLUME_MAX)
            menu.value = TUNER_VOLUME_MAX;
        if (menu.value < TUNER_VOLUME_MIN)
            menu.value = TUNER_VOLUME_MIN;
        break;
    default:
        break;
    }
}

static bool menuIsValid(MenuIdx index)
{
    Tuner *tuner = tunerGet();

    switch (index) {
    case MENU_NULL:
        // Don't allow null menu
        return false;
    case MENU_TUNER_RDS:
        switch (tuner->par.ic) {
        case TUNER_IC_RDA5807:
        case TUNER_IC_SI4703:
            break;
        default:
            return false;
        }
        break;
    case MENU_TUNER_BASS:
        switch (tuner->par.ic) {
        case TUNER_IC_RDA5807:
            break;
        default:
            return false;
        }
        break;
    case MENU_TUNER_VOLUME:
        switch (tuner->par.ic) {
        case TUNER_IC_RDA5807:
        case TUNER_IC_SI4703:
            break;
        default:
            return false;
        }
        break;
    }

    return true;
}

static void menuSelect(MenuIdx index)
{
    menu.selected = 0;

    menu.active = (index != MENU_NULL) ? index : menu.parent;
    menu.parent = menuItems[index].parent;
    menu.dispOft = 0;

    uint8_t idx;

    for (idx = 0; idx < MENU_MAX_LEN; idx++) {
        menu.list[idx] = 0;
    }

    idx = 0;
    if (!menuIsTop()) {
        menu.list[idx++] = MENU_NULL;
        menu.active = MENU_NULL;
    }
    for (MenuIdx item = 0; item < MENU_END; item++) {
        if ((menuItems[item].parent == menu.parent) && menuIsValid(item)) {
            menu.list[idx++] = (uint8_t)item;
            if (idx >= MENU_MAX_LEN) {
                break;
            }
        }
    }

    menu.listSize = idx;
}

static void menuUpdate(MenuIdx index)
{
    switch (index) {
    case MENU_TUNER_IC:
        tunerReadSettings();
        break;
    default:
        return;
    }

    screenToClear();
    menuSelect(index);
    menu.active = index;
}

Menu *menuGet(void)
{
    return &menu;
}

void menuSetActive(MenuIdx index)
{
    if (menu.active == index) {
        menu.selected = !menu.selected;

        if (menu.selected) {
            menu.value = menuGetValue(menu.active);
        } else {
            menuStoreCurrentValue();
            menuUpdate(index);
        }

        return;
    }

    menuSelect(index);
}

void menuChange(int8_t diff)
{
    if (diff > 0)
        diff = 1;
    else if (diff < 0)
        diff = -1;

    if (menu.selected) {
        menuValueChange(diff);
    } else {
        menuMove(diff);
    }
}

bool menuIsTop(void)
{
    // TODO: Top menu on first selection instead of MENU_SETUP
    return (menu.parent == MENU_NULL || menu.parent == MENU_SETUP);
}

MenuIdx menuGetFirstChild(void)
{
    if (menu.active == MENU_NULL)
        return menu.parent;

    for (MenuIdx item = 0; item < MENU_END; item++) {
        if (menuItems[item].parent == menu.active) {
            return item;
        }
    }
    return menu.active;
}

__attribute__ ((noinline))
const char *menuGetName(MenuIdx index)
{
    char *name;

    name = utilMkStr("%s", labelsGet((Label)(LABEL_MENU + (index - MENU_NULL))));

    return name;
}

const char *menuGetValueStr(MenuIdx index)
{
    const char *ret = ">";
    static const char *noVal = "--";

    // Parent menu type
    if (menuItems[index].type == MENU_TYPE_PARENT) {
        return (index == MENU_NULL) ? "" : ret;
    }

    int16_t value = menuGetValue(index);

    if (index == menu.active && menu.selected)
        value = menu.value;

    // Bool menu type
    if (menuItems[index].type == MENU_TYPE_BOOL) {
        ret = labelsGet((Label)(LABEL_BOOL_OFF + value));
        return ret;
    }

    if (menuItems[index].type == MENU_TYPE_NUMBER) {
        ret = utilMkStr("%5d", value);
        return ret;
    }

    if (menuItems[index].type == MENU_TYPE_RC) {
        if ((uint16_t)value == EE_NOT_FOUND) {
            ret = noVal;
        } else {
            ret = utilMkStr("%04d", (uint16_t)value);
        }
        return ret;
    }

    // Enum menu types
    switch (index) {
    case MENU_TUNER_IC:
        ret = labelsGet((Label)(LABEL_TUNER_IC + value));
        break;
    case MENU_TUNER_BAND:
        ret = labelsGet((Label)(LABEL_TUNER_BAND + value));
        break;
    case MENU_TUNER_STEP:
        ret = labelsGet((Label)(LABEL_TUNER_STEP + value));
        break;
    case MENU_TUNER_DEEMPH:
        ret = labelsGet((Label)(LABEL_TUNER_DEEMPH + value));
        break;
    default:
        ret = noVal;
        break;
    }

    return ret;
}
