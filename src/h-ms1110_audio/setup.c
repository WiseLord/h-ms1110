#include "setup.h"

#include "rtc.h"

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
        setup.child = SETUP_TIME_HOUR;
        break;
    case SETUP_DATE:
        setup.child = SETUP_DATE_DAY;
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

    switch (setup.child) {
    case SETUP_TIME ... SETUP_REMOTE:
        first = SETUP_TIME;
        last = SETUP_REMOTE;
        break;
    case SETUP_TIME_HOUR ... SETUP_TIME_SECOND:
        first = SETUP_TIME_HOUR;
        last = SETUP_TIME_SECOND;
        break;
    case SETUP_DATE_DAY ... SETUP_DATE_YEAR:
        first = SETUP_DATE_DAY;
        last = SETUP_DATE_YEAR;
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
    switch (setup.child) {
    case SETUP_TIME_HOUR:
        rtcChangeTime(RTC_HOUR, direction);
        break;
    case SETUP_TIME_MINUTE:
        rtcChangeTime(RTC_MIN, direction);
        break;
    case SETUP_TIME_SECOND:
        rtcChangeTime(RTC_SEC, direction);
        break;
    case SETUP_DATE_DAY:
        rtcChangeTime(RTC_DATE, direction);
        break;
    case SETUP_DATE_MONTH:
        rtcChangeTime(RTC_MONTH, direction);
        break;
    case SETUP_DATE_YEAR:
        rtcChangeTime(RTC_YEAR, direction);
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
