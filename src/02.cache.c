// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "helper.h"
#include "pthread.h"

#define NUM_MEASURES 25
#define BUFF_LEN 1024

int orig[BUFF_LEN];
int dest[BUFF_LEN];
pthread_mutex_t lock;

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

void* copy_thread(void *arg)
{
	int times[NUM_MEASURES];
	int final;
	int avg;
	float inc;

	pthread_mutex_lock(&lock);
	printf("> In copy thread\n");

	// skip first access
	timed_memcpy(dest, orig, sizeof(orig));
	for (int i=0; i<NUM_MEASURES; i++) {
		times[i] = timed_memcpy(dest, orig, sizeof(orig));
	}

	pthread_mutex_unlock(&lock);
	usleep(1);
	pthread_mutex_lock(&lock);
	printf("> Continue copy thread\n");

	final = timed_memcpy(dest, orig, sizeof(orig));

	avg = calculate_avg(times);
	printf("+ Calculated delta average: %d ticks\n", avg);
	inc = (final - avg) * 100 / avg;
	printf("+ Access delta: %d ticks. Increase: %.2f%\n", final, inc);

	pthread_mutex_unlock(&lock);

	return 0;
}

int main (int argc, char **argv)
{
	pthread_t id[2];
	if (pthread_mutex_init(&lock, NULL) != 0) {
        	printf("mutex init failed\n");
		return 1;
	}
	pthread_create(&id[0], NULL, copy_thread, NULL);
	usleep(1);
	pthread_mutex_lock(&lock);
	printf("> In main thread\n");
	flush(orig);
	printf("+ Flush\n");
	pthread_mutex_unlock(&lock);

	pthread_join(id[0], NULL);
}
