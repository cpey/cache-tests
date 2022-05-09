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
#define NUM_TESTS 	1000
#define BUFF_LEN 	2048
#define CACHE_LINE_LEN 	64
#define RAND_SEQ_LEN 	4
#define SEARCH_LEN 	RAND_SEQ_LEN

int data[BUFF_LEN];
sem_t sem_search, sem_go;

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
	uint32_t *rseq;
	volatile uint8_t dummy;

	rseq = get_rand_seq(RAND_SEQ_LEN, BUFF_LEN);
	print_seq(rseq, RAND_SEQ_LEN);
	for (int i=0; i<NUM_TESTS; i++) {
		sem_wait(&sem_go);
		for (int j=0; j<RAND_SEQ_LEN; j++) {
			dummy = data[rseq[j]];
		}
		sem_post(&sem_search);
	}
	free(rseq);
}

uint16_t run_search()
{
	uint64_t delta;
	ticks s_delta[SEARCH_LEN];
	uint16_t s_pos[SEARCH_LEN];
	uint8_t min, max;	

	memset(s_delta, UCHAR_MAX, SEARCH_LEN * sizeof(ticks));

	for (int i=0; i<BUFF_LEN; i+=CACHE_LINE_LEN) {
		delta = timed_read((uint8_t *) data, i+10);	
		max = get_largest_idx(s_delta, SEARCH_LEN);
		mprintf("idx: %d, delta: %u\n", i, max, delta);
		if (delta < s_delta[max]) {
			s_delta[max] = delta;
			s_pos[max] = i;
		}
	}

	printf("+ Found pos: ");
	printf(" %d", s_pos[0]);
	for (int i=1; i<SEARCH_LEN; i++) {
		printf(", %d", s_pos[i]);	
	}
	printf("\n");
	min = get_smallest_idx(s_delta, SEARCH_LEN);
	return s_pos[min];
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
		pos = run_search();
		flush(data);
		usleep(500);
		sem_post(&sem_go);
	}

	pthread_join(id, NULL);
}
