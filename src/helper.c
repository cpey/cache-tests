// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "helper.h"

static inline ticks getticks(void)
{
     unsigned int a, d;
     asm volatile("rdtsc" : "=a" (a), "=d" (d));

     return (((ticks) a) | (((ticks)d) << 32));
}

int timed_memcpy(int *dest, const int *orig, int size)
{
	ticks ticks1, ticks2;

	ticks1 = getticks();
	memcpy(dest, orig, size);
	ticks2 = getticks();
	return (ticks2 - ticks1);
}

int timed_read(uint8_t *arr, int pos)
{
	ticks ticks1, ticks2;
	uint8_t dummy;

	ticks1 = getticks();
	dummy = arr[pos];
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

uint16_t get_smallest_idx(ticks *data, unsigned len)
{
	uint16_t min = 0;

	for (int i=1; i<len; i++) {
		if (data[i] < data[min]) {
			min = i;
		}
	}

	return min;
}

uint16_t get_largest_idx(ticks *data, unsigned len)
{
	uint16_t min = 0;

	for (int i=1; i<len; i++) {
		if (data[i] > data[min]) {
			min = i;
		}
	}

	return min;
}

uint8_t *get_rand_seq(unsigned len)
{
	uint8_t *seq;
	time_t t;

	seq = malloc(len * sizeof(uint8_t));
	srand((unsigned) time(&t));
	for (int i; i<len; i++) {
		seq[i] = rand() % (uint32_t) pow(2, 8*sizeof(seq[0]));
	}
	return seq;
}
