#include "layout.h"

static const Layout lt256x64 = {
    .lblFont = &fontterminus28b,
};

const Layout *layoutGet(void)
{
    return &lt256x64;
}
