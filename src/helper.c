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


/*
 * https://eprint.iacr.org/2013/448.pdf
 */
unsigned long probe(char *addr)
{
	volatile unsigned long time;

	asm __volatile__ (
	  " mfence 		\n"
	  " lfence 		\n"
	  " rdtsc 		\n"
	  " lfence 		\n"
	  " movl %%eax, %%esi 	\n"
	  " movl (%1), %%eax 	\n"
	  " lfence 		\n"
	  " rdtsc 		\n"
	  " subl %%esi, %%eax 	\n"
	  " clflush 0(%1) 	\n"
	  : "=a" (time)
	  : "c" (addr)
	  : "%esi", "%edx");
	return time;
}

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
	volatile uint8_t dummy;

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

void mprint_arr(int32_t *arr, unsigned len, char *str)
{
	if (str) {
		mprintf("%s ", str);
	}
	mprintf("%d", arr[0]);
	for (int i=1; i<len; i++) {
		mprintf(", %d", arr[i]);
	}
	mprintf("\n");
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

uint32_t *get_rand_seq(unsigned len, unsigned limit)
{
	uint32_t *seq;
	time_t t;

	seq = malloc(len * sizeof(uint8_t));
	srand((unsigned) time(&t));
	for (int i; i<len; i++) {
		seq[i] = rand() % (uint32_t) limit;
	}
	return seq;
}

void flush_arr(uint8_t *data, unsigned len)
{
	for (int i=0; i<len; i+=CACHE_LINE_LEN) {
		flush(data+i);
	}
}

void sort(int *arr, int first, int last)
{
	int i, j, pivot, tmp;

	if (first > last) {
		return;
	}

	pivot=first;
	i=first;
	j=last;

	while (i < j) {
		while (arr[i] <= arr[pivot] && i < last)
			i++;
		while (arr[j] > arr[pivot])
			j--;
		if (i < j) {
			tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp;
		}
	}

	tmp = arr[pivot];
	arr[pivot] = arr[j];
	arr[j] = tmp;
	sort(arr, first, j-1);
	sort(arr, j+1, last);
}

int sum_abs_diffs(uint32_t *arr1, uint32_t *arr2, unsigned len)
{
	int res;

	for (int i=0; i<len; i++) {
		res += abs(arr1[i] - arr2[i]);
	}

	return res;
}
