// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "helper.h"

#define NUM_MEASURES 	25
#define NUM_TESTS 	100
#define BUFF_LEN 	1024

int orig[BUFF_LEN];
int dest[BUFF_LEN];
sem_t sem_flush, sem_copy;

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
		mprintf("T%d: %d\n", i, meas[i]);
	}
}

int run_test()
{
	int times[NUM_MEASURES];
	int final;
	int avg;
	float inc;

	sem_wait(&sem_copy);
	mprintf("> In copy thread\n");

	// skip first access
	timed_memcpy(dest, orig, sizeof(orig));
	for (int i=0; i<NUM_MEASURES; i++) {
		times[i] = timed_memcpy(dest, orig, sizeof(orig));
	}

	sem_post(&sem_flush);
	sem_wait(&sem_copy);
	mprintf("> Continue copy thread\n");

	final = timed_memcpy(dest, orig, sizeof(orig));

	avg = calculate_avg(times);
	mprintf("+ Calculated delta average: %d ticks\n", avg);
	inc = (final - avg) * 100 / avg;
	mprintf("+ Access delta: %d ticks. Increase: %.2f%\n", final, inc);
	sem_post(&sem_copy);

	return inc;
}

void* copy_thread(void *arg)
{
	uint64_t avg = 0;

	for (int i=0; i<NUM_TESTS; i++) {
		avg += run_test();
	}
	printf("** Average increase: %.2f%\n", avg / (float)NUM_TESTS);
}

int main (int argc, char **argv)
{
	pthread_t id[2];
	bool do_flush = TRUE;

	if (argc == 2 && !strcmp(argv[1], "--no-flush")) {
		do_flush = FALSE;
	}

	if (sem_init(&sem_flush, 0, 0) != 0) {
        	mprintf("Sempahore init failed\n");
		return 1;
	}

	if (sem_init(&sem_copy, 0, 1) != 0) {
        	mprintf("Sempahore init failed\n");
		return 1;
	}

	pthread_create(&id[0], NULL, copy_thread, NULL);
	for (int i=0; i<NUM_TESTS; i++) {
		sem_wait(&sem_flush);
		mprintf("> In main thread\n");
		if (do_flush) {
			flush(orig);
			mprintf("+ Flush\n");
		}
		sem_post(&sem_copy);
	}

	pthread_join(id[0], NULL);
}
