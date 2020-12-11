#ifndef SETUP_H
#define SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int16_t SetupType;
enum {
    SETUP_NULL = 0,

    SETUP_MAIN,

    SETUP_TIME,
    SETUP_DATE,
    SETUP_ALARM,
    SETUP_REMOTE,

    SETUP_HEAD_LAST = SETUP_REMOTE,

    SETUP_END
};

typedef struct {
    SetupType active;
    SetupType child;
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