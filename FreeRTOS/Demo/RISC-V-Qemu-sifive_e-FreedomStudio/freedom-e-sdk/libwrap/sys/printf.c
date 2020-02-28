/* See LICENSE of license details. */

#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include "weak_under_alias.h"
#include "puts.h"

int __wrap_printf(const char *s, ...)
{
    char buf[256];
    va_list vl;

    const char *p = &buf[0];

    va_start(vl, s);
    vsnprintf(buf, sizeof buf, s, vl);
    va_end(vl);

    __wrap_puts(p);

    return 0;
}
weak_under_alias(printf);
