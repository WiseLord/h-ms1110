#include "layout.h"

static const Layout lt256x64 = {
    .par = {
        .valFont = &fontterminus32,
        .valX = 0,
        .valY = 0,
    },
};

const Layout *layoutGet(void)
{
    return &lt256x64;
}
