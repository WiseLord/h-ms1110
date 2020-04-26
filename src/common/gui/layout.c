#include "layout.h"

static const Layout lt256x64 = {
    .tune = {
        .value.font = &fontterminus32,
        .bar.y = 44,
    },
};

const Layout *layoutGet(void)
{
    return &lt256x64;
}
