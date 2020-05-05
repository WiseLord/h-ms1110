#include "utils.h"

#include "hwlibs.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void utilInitSysCounter(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void utilmDelay(uint32_t ms)
{
    LL_mDelay(ms);
}

void utiluDelay(uint32_t us)
{
    uint32_t tickNow = DWT->CYCCNT;
    uint32_t ticksInMhz = SystemCoreClock / 1000000;
    uint32_t ticksWait = us * ticksInMhz;

    while (DWT->CYCCNT - tickNow < ticksWait);
}

// TRUE if end of line
bool utilReadChar(LineParse *lp, char ch)
{
    if (lp->idx == 0) {
        lp->size = 0;
    }

    if (lp->idx + 2 >= LINE_SIZE) {
        lp->line[lp->idx + 1] = ch;
        lp->idx = 0;
        return true;
    } else if (ch == '\n') {
        lp->line[lp->idx] = '\0';
        lp->idx = 0;
        return true;
    } else {
        lp->line[lp->idx++] = ch;
        lp->size++;
        return false;
    }
}

bool utilIsPrefix(const char *line, const char *prefix)
{
    char p;
    char l = '\0';

    bool ret = true;

    while (line && (p = *prefix++) && (l = *line++)) {
        if (p != l) {
            ret = false;
            break;
        }
    }

    if (!l) {
        ret = false;
    }

    return ret;
}

void utilTrimLineEnd(char *line)
{
    size_t len = strlen(line);

    // Remove possible service symbols and spaces at the end
    while (len > 0 && (line[len - 1] <= ' ')) {
        line[len - 1] = '\0';
        len--;
    }
}
