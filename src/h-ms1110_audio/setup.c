#include "setup.h"

static Setup setup;


Setup *setupGet()
{
    return &setup;
}

void setupSelect(SetupType type)
{
    if (type >= SETUP_END) {
        type = SETUP_MAIN;
    }

    if (type <= SETUP_HEAD_LAST) {
        setup.head = type;
    }

    switch (setup.head) {
    case SETUP_MAIN:
        setup.active = SETUP_TIME;
        break;
    }
}

void setupExit()
{
    setup.head = SETUP_MAIN;
    setup.active = SETUP_TIME;
}

void setupMove (int8_t direction)
{
    SetupType first = SETUP_MAIN;
    SetupType last = SETUP_MAIN;

    switch (setup.active) {
    case SETUP_TIME:
    case SETUP_ALARM:
    case SETUP_REMOTE:
        first = SETUP_TIME;
        last = SETUP_REMOTE;
        break;
    }

    setup.active += direction;

    if (setup.active < first) {
        setup.active = first;
    }
    if (setup.active > last) {
        setup.active = last;
    }
}
