#ifndef SETUP_H
#define SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t SetupType;
enum {
    SETUP_MAIN = 0,

    SETUP_TIME,
    SETUP_ALARM,
    SETUP_REMOTE,

    SETUP_END
};

typedef struct {
    SetupType parent;
    SetupType active;
} Setup;

Setup *setupGet(void);

void setupPrev(void);
void setupNext(void);
void setupSelect(void);
void setupExit(void);

#ifdef __cplusplus
}
#endif

#endif // SETUP_H
