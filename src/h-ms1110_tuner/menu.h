#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "cmd.h"

#define GENERATE_MENU_RC(CMD)    MENU_RC_ ## CMD,

typedef uint8_t MenuIdx;
enum {
    MENU_NULL = 0,

    MENU_SETUP,

    MENU_SETUP_SYSTEM,
    MENU_SETUP_TUNER,

    MENU_SYSTEM_ENC_RES,
    MENU_SYSTEM_RTC_CORR,

    MENU_TUNER_IC,
    MENU_TUNER_BAND,
    MENU_TUNER_STEP,
    MENU_TUNER_DEEMPH,
    MENU_TUNER_STA_MODE,
    MENU_TUNER_FMONO,
    MENU_TUNER_RDS,
    MENU_TUNER_BASS,
    MENU_TUNER_VOLUME,

    MENU_END,
};

#define MENU_MAX_LEN    MENU_END

typedef uint8_t MenuType;
enum {
    MENU_TYPE_PARENT,
    MENU_TYPE_BOOL,
    MENU_TYPE_NUMBER,
    MENU_TYPE_ENUM,
    MENU_TYPE_RC,

    MENU_TYPE_END
};

typedef struct {
    MenuIdx parent;
    MenuIdx active;
    uint8_t list[MENU_MAX_LEN];
    uint8_t listSize;       // Total number of items in current menu
    uint8_t selected;
    int16_t value;

    uint8_t dispSize;       // Number of items display able to show
    int8_t dispOft;         // First visible item offset on display
} Menu;

Menu *menuGet(void);

void menuSetActive(MenuIdx index);
void menuChange(int8_t diff);
bool menuIsTop(void);

MenuIdx menuGetFirstChild(void);

const char *menuGetName(MenuIdx index);
const char *menuGetValueStr(MenuIdx index);

#ifdef __cplusplus
}
#endif

#endif // MENU_H
