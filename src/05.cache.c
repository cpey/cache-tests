// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "helper.h"

#define NUM_MEASURES 	25
#define NUM_TESTS 	3
#define BUFF_LEN 	2048
#define RAND_SEQ_LEN 	4
#define SEARCH_LEN 	RAND_SEQ_LEN

uint8_t data[BUFF_LEN];
sem_t sem_search, sem_go;
uint32_t *rand_seq;
float avg_diff;

void print_seq(uint32_t *seq, int len)
{
	printf("+ Seq: %d", seq[0]);
	for (int i=1; i<len; i++) {
		printf(", %d", seq[i]);
	}
	printf("\n");
}

void *victim(void *arg)
{
	uint64_t avg = 0;
	volatile uint8_t dummy;

	rand_seq = get_rand_seq(RAND_SEQ_LEN, BUFF_LEN);
	sort(rand_seq, 0, RAND_SEQ_LEN-1);
	print_seq(rand_seq, RAND_SEQ_LEN);
	for (int i=0; i<NUM_TESTS; i++) {
		sem_wait(&sem_go);
		for (int j=0; j<RAND_SEQ_LEN; j++) {
			dummy = data[rand_seq[j]];
		}
		sem_post(&sem_search);
	}
}

void run_search()
{
	uint64_t delta;
	ticks s_delta[SEARCH_LEN];
	int32_t s_pos[SEARCH_LEN];
	uint8_t min, max;	
	uint32_t diff;
	uint8_t d;

	memset(s_delta, UCHAR_MAX, SEARCH_LEN * sizeof(ticks));

	for (int i=0; i<BUFF_LEN; i+=CACHE_LINE_LEN) {
		delta = probe((uint8_t *) (data + i));
		max = get_largest_idx(s_delta, SEARCH_LEN);
		mprintf("idx: %d, delta: %u\n", i, delta);
		if (delta < s_delta[max]) {
			s_delta[max] = delta;
			s_pos[max] = i;
		}
	}

	sort(s_pos, 0, SEARCH_LEN - 1);
	diff = sum_abs_diffs(rand_seq, s_pos, SEARCH_LEN);
	mprint_arr(s_pos, SEARCH_LEN, "+ Found pos:");
	mprintf("  -> diff: %d\n", diff);
	avg_diff += diff / NUM_TESTS;
}

int main (int argc, char **argv)
{
	pthread_t id;
	uint16_t pos;

	if (sem_init(&sem_search, 0, 0) != 0) {
        	mprintf("Semaphore init failed\n");
		return 1;
	}

	if (sem_init(&sem_go, 0, 1) != 0) {
        	mprintf("Semaphore init failed\n");
		return 1;
	}

	pthread_create(&id, NULL, victim, NULL);
	for (int i=0; i<NUM_TESTS; i++) {
		sem_wait(&sem_search);
		run_search();
		sem_post(&sem_go);
	}

	pthread_join(id, NULL);
	printf("Average diff: %.02f\n", avg_diff);
}
