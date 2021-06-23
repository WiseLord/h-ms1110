#ifndef SETUP_H
#define SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int8_t SetupType;
enum {
    SETUP_NULL = 0,

    SETUP_MAIN,

    SETUP_TIME,
    SETUP_DATE,
    SETUP_TIMECORR,
    SETUP_ALARM,
    SETUP_REMOTE,

    SETUP_HEAD_LAST = SETUP_REMOTE,

    SETUP_END
};

typedef uint8_t SetupFlags;
enum {
    SETUP_FLAG_NONE             = 0x00,

    SETUP_FLAG_ACTIVE_CHANGED   = 0x01,
    SETUP_FLAG_CHILD_CHANGED    = 0x02,
};

typedef struct {
    SetupType active;
    SetupType child;
    SetupFlags flags;
} Setup;

Setup *setupGet(void);

void setupSelect(SetupType type);
void setupSwitchChild(int8_t direction);
void setupChangeChild(int8_t direction);
void setupBack(void);

#ifdef __cplusplus
}
#endif

#endif // SETUP_H
