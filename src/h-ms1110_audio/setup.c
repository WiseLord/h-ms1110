#include "setup.h"

#include "rtc.h"
#include "settings.h"

static Setup setup;

Setup *setupGet()
{
    return &setup;
}


void setupSelect(SetupType type)
{
    if (type >= SETUP_END) {
        type = SETUP_NULL;
    }

    if (type <= SETUP_HEAD_LAST) {
        setup.active = type;
    }

    switch (setup.active) {
    case SETUP_NULL:
        setup.child = SETUP_MAIN;
        break;
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
    }
}

void setupBack()
{
    SetupType active = setup.active;

    switch (active) {
    case SETUP_TIME:
    case SETUP_ALARM:
    case SETUP_REMOTE:
        setupSelect(SETUP_MAIN);
        setup.child = active;
        break;
    default:
        setupSelect(SETUP_NULL);
        break;
    }
}
