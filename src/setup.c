#include "setup.h"

#include "rc.h"
#include "rtc.h"
#include "settings.h"

static Setup setup;

Setup *setupGet()
{
    return &setup;
}

static void updateActive(SetupType value)
{
    if (setup.active != value) {
        setup.flags |= SETUP_FLAG_ACTIVE_CHANGED;
        setup.active = value;
    }
}

static void updateChild(SetupType value)
{
    if (setup.child != value) {
        setup.flags |= SETUP_FLAG_CHILD_CHANGED;
        setup.child = value;
    }
}

static void updateSettings(SetupType type)
{
    switch (type) {
    case SETUP_TIMECORR: {
        int16_t corr = rtcGetCorrection();
        settingsStore(PARAM_SYSTEM_RTC_CORR, corr);
    }
    break;
    case SETUP_ALARM: {
        Alarm *alarm = rtcGetAlarm(0);
        settingsStore(PARAM_ALARM_HOUR, alarm->hour);
        settingsStore(PARAM_ALARM_MINUTE, alarm->min);
        settingsStore(PARAM_ALARM_DAYS, alarm->days);
    }
    break;
    }
}

void setupSelect(SetupType type)
{
    if (type == SETUP_NULL || type > SETUP_HEAD_LAST) {
        return;
    }

    SetupType child = SETUP_NULL;

    switch (type) {
    case SETUP_MAIN:
        child = SETUP_TIME;
        break;
    case SETUP_TIME:
        child = RTC_HOUR;
        break;
    case SETUP_DATE:
        child = RTC_DATE;
        break;
    case SETUP_TIMECORR:
        child = 0;
        break;
    case SETUP_ALARM:
        child = ALARM_DAYS;
        break;
    case SETUP_REMOTE:
        child = RC_CMD_STBY_SWITCH;
        break;
    }

    updateActive(type);
    updateChild(child);
}

void setupSwitchChild(int8_t direction)
{
    SetupType active = setup.active;
    SetupType child = setup.child;

    SetupType first = SETUP_NULL;
    SetupType last = SETUP_NULL;

    child += direction;

    switch (active) {
    case SETUP_MAIN:
        first = SETUP_TIME;
        last = SETUP_REMOTE;
        break;
    case SETUP_TIME:
    case SETUP_DATE:
        first = RTC_HOUR;
        last = RTC_YEAR;
        break;
    case SETUP_TIMECORR:
        first = 0;
        last = 0;
        break;
    case SETUP_ALARM:
        first = ALARM_HOUR;
        last = ALARM_DAYS;
        break;
    case SETUP_REMOTE:
        first = RC_CMD_STBY_SWITCH;
        last = RC_CMD_END - 1;
        break;
    default:
        return;
    }

    if (child < first) {
        child = last;
    }
    if (child > last) {
        child = first;
    }

    if (active == SETUP_TIME || active == SETUP_DATE) {
        active = (child <= RTC_SEC) ? SETUP_TIME : SETUP_DATE;
    }

    updateActive(active);
    updateChild(child);
}

void setupChangeChild(int8_t direction)
{
    switch (setup.active) {
    case SETUP_TIME:
    case SETUP_DATE:
        rtcChangeTime(setup.child, direction);
        break;
    case SETUP_TIMECORR:
        rtcSetCorrection(rtcGetCorrection() + direction);
        break;
    case SETUP_ALARM:
        rtcChangeAlarm(setup.child, direction);
        break;
    case SETUP_REMOTE:
        if (setup.child >= RC_CMD_STBY_SWITCH && setup.child < RC_CMD_END) {
            RcData rcData = rcRead(false);
            uint16_t raw = (int16_t)(((rcData.addr & 0xFF) << 8) | rcData.cmd);
            rcSaveCode(setup.child, raw);
        }
        break;
    }
}

void setupBack()
{
    SetupType active = setup.active;
    SetupType child = setup.child;

    switch (active) {
    case SETUP_TIME:
    case SETUP_DATE:
    case SETUP_TIMECORR:
    case SETUP_ALARM:
    case SETUP_REMOTE:
        child = active;
        updateSettings(active);
        active = SETUP_MAIN;
        break;
    default:
        child = SETUP_NULL;
        active = SETUP_NULL;
        break;
    }

    updateActive(active);
    updateChild(child);
}
