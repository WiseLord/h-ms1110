#ifndef SETUP_H
#define SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int8_t SetupType;
enum {
    SETUP_MAIN = 0,

    SETUP_TIME,
    SETUP_ALARM,
    SETUP_REMOTE,

    SETUP_TIME_HOUR,
    SETUP_TIME_MINUTE,
    SETUP_TIME_SECOND,
    SETUP_TIME_DATE,
    SETUP_TIME_MONTH,
    SETUP_TIME_YEAR,

    SETUP_END
};

typedef struct {
    SetupType head;
    SetupType active;
} Setup;

Setup *setupGet(void);

void setupSelect(SetupType type);
void setupExit(void);

void setupMove (int8_t direction);

#ifdef __cplusplus
}
#endif

#endif // SETUP_H
