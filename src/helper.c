// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"

static inline uint64_t getticks(void)
{
     unsigned int a, d;
     asm volatile("rdtsc" : "=a" (a), "=d" (d));

     return (((uint64_t) a) | (((uint64_t)d) << 32));
}

void flush(void *addr)
{
     asm volatile("clflush (%0)" :: "r"(addr));
}

int timed_memcpy(int *dest, const int *orig, int size)
{
	uint64_t ticks1, ticks2;

	ticks1 = getticks();
	memcpy(dest, orig, size);
	ticks2 = getticks();
	return (ticks2 - ticks1);
}

int mprintf(const char *fmt, ...)
{
#ifndef MUTE
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
#endif
}
