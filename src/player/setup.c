#include "setup.h"

#include "rc.h"
#include "rtc.h"
#include "settings.h"

static Setup setup;

Setup *setupGet()
{
    return &setup;
}


void setupSelect(SetupType type)
{
    if (type == SETUP_NULL || type > SETUP_HEAD_LAST) {
        return;
    }

    setup.active = type;

    switch (setup.active) {
    case SETUP_MAIN:
        setup.child = SETUP_TIME;
        break;
    case SETUP_TIME:
        setup.child = RTC_HOUR;
        break;
    case SETUP_DATE:
        setup.child = RTC_DATE;
        break;
    case SETUP_ALARM:
        setup.child = ALARM_DAYS;
        break;
    case SETUP_REMOTE:
        setup.child = RC_CMD_STBY_SWITCH;
        break;
    default:
        setup.child = SETUP_NULL;
        break;
    }
}

void setupSwitchChild(int8_t direction)
{
    SetupType first = SETUP_NULL;
    SetupType last = SETUP_NULL;

    switch (setup.active) {
    case SETUP_MAIN:
        first = SETUP_TIME;
        last = SETUP_REMOTE;
        break;
    case SETUP_TIME:
        first = RTC_HOUR;
        last = RTC_SEC;
        break;
    case SETUP_DATE:
        first = RTC_DATE;
        last = RTC_YEAR;
        break;
    case SETUP_ALARM:
        first = ALARM_HOUR;
        last = ALARM_DAYS;
        break;
    case SETUP_REMOTE:
        first = RC_CMD_STBY_SWITCH;
        last = RC_CMD_SCR_DEF;
        break;
    default:
        return;
    }

    setup.child += direction;

    if (setup.child < first) {
        setup.child = first;
    }
    if (setup.child > last) {
        setup.child = last;
    }
}

void setupChangeChild(int8_t direction)
{
    switch (setup.active) {
    case SETUP_TIME:
    case SETUP_DATE:
        rtcChangeTime(setup.child, direction);
        break;
    case SETUP_ALARM:
        rtcChangeAlarm(setup.child, direction);
        Alarm *alarm = rtcGetAlarm(0);
        switch (setup.child) {
        case ALARM_HOUR:
            settingsStore(PARAM_ALARM_HOUR, alarm->hour);
            break;
        case ALARM_MIN:
            settingsStore(PARAM_ALARM_MINUTE, alarm->min);
            break;
        case ALARM_DAYS:
            settingsStore(PARAM_ALARM_DAYS, alarm->days);
            break;
        }
        break;
    case SETUP_REMOTE:
        if (setup.child >= RC_CMD_STBY_SWITCH && setup.child <= RC_CMD_SCR_DEF) {
            RcData rcData = rcRead(false);
            uint16_t raw = (int16_t)(((rcData.addr & 0xFF) << 8) | rcData.cmd);
            Param param = PARAM_RC_STBY_SWITCH + setup.child;
            settingsStore(param, raw);
        }
        break;
    }
}

void setupBack()
{
    SetupType active = setup.active;

    switch (active) {
    case SETUP_TIME:
    case SETUP_DATE:
    case SETUP_ALARM:
    case SETUP_REMOTE:
        setup.active = SETUP_MAIN;
        setup.child = active;
        break;
    default:
        setup.active = SETUP_NULL;
        setup.child = SETUP_NULL;
        break;
    }
}
