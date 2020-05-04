#ifndef SPECTRUM_H
#define SPECTRUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "fft.h"

typedef uint8_t SpChan;
enum {
    SP_CHAN_LEFT = 0,
    SP_CHAN_RIGHT,
    SP_CHAN_BOTH,

    SP_CHAN_END = SP_CHAN_BOTH
};

typedef int8_t SpMode;
enum {
    SP_MODE_STEREO = 0,
    SP_MODE_MIRROR,
    SP_MODE_ANTIMIRROR,

    SP_MODE_STEREO_END = SP_MODE_ANTIMIRROR,

    SP_MODE_MIXED,
    SP_MODE_LEFT,
    SP_MODE_RIGHT,

    SP_MODE_MIXED_MIRROR,
    SP_MODE_LEFT_MIRROR,
    SP_MODE_RIGHT_MIRROR,

    SP_MODE_WATERFALL,

    SP_MODE_END
};

#define N_DB            256

typedef struct {
    SpMode mode;
    bool peaks;
    bool grad;
} Spectrum;

// Callback to convert FFT data
typedef void (*fftGet)(FftSample *sp, uint8_t *out, size_t size);

void spInit(void);
Spectrum *spGet(void);

uint8_t spGetDb(uint16_t value, uint8_t min, uint8_t max);

void spGetADC(SpChan chan, uint8_t *out, size_t size, fftGet fn);

void spConvertADC(void);

bool spCheckSignal(void);

#ifdef __cplusplus
}
#endif

#endif // SPECTRUM_H
