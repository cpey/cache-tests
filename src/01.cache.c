// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "helper.h"

#define NUM_MEASURES 25
#define BUFF_LEN 1024

int calculate_avg(int *meas)
{
	int avg = 0;

	for (int i=0; i<NUM_MEASURES; i++) {
		avg += meas[i];
	}
	avg /= NUM_MEASURES;
	return avg;
}

void print_measures(int *meas) 
{
	for (int i=0; i<NUM_MEASURES; i++) {
		printf("T%d: %d\n", i, meas[i]);
	}
}

int main (int argc, char **argv)
{
	int orig[BUFF_LEN];
	int dest[BUFF_LEN];
	int times[NUM_MEASURES];
	int final;
	int avg;
	float inc;

	// skip first access
	timed_memcpy(dest, orig, sizeof(orig));
	for (int i=0; i<NUM_MEASURES; i++) {
		times[i] = timed_memcpy(dest, orig, sizeof(orig));
	}
	flush(orig);
	final = timed_memcpy(dest, orig, sizeof(orig));
	
	avg = calculate_avg(times);
	printf("+ Calculated delta average: %d ticks\n", avg);
	inc = (final - avg) * 100 / avg;
	printf("+ Delta flushed access: %d ticks. Increase: %.2f%\n", final, inc);
}
