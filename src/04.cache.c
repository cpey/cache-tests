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
#define RAND_SEQ_LEN 	16
#define BUFF_LEN 	1024
#define SEARCH_LEN 	5

int data[BUFF_LEN];
sem_t sem_search, sem_go;

void print_seq(uint8_t *seq, int len)
{
	printf("Seq: %d", seq[0]);
	for (int i=1; i<len; i++) {
		printf(", %d", seq[i]);
	}
	printf("\n");
}

void *victim(void *arg)
{
	uint64_t avg = 0;
	uint8_t *rseq;
	uint8_t dummy;

	rseq = get_rand_seq(RAND_SEQ_LEN);
	print_seq(rseq, RAND_SEQ_LEN);
	for (int i=0; i<NUM_TESTS; i++) {
		for (int j=0; j<RAND_SEQ_LEN; j++) {
			sem_wait(&sem_go);
			dummy = data[rseq[j]];
			sem_post(&sem_search);
		}
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

	for (int i=0; i<BUFF_LEN; i++) {
		delta = timed_read((uint8_t *) data, i);	
		max = get_largest_idx(s_delta, SEARCH_LEN);
		if (delta < s_delta[max]) {
			s_delta[max] = delta;
			s_pos[max] = i;
		}
	}

	printf(" + Found pos: ");	
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
		flush(data);
		for (int j=0; j<RAND_SEQ_LEN; j++) {
			sem_wait(&sem_search);
			pos = run_search();
			flush(data);
			sem_post(&sem_go);
		}
	}

	pthread_join(id, NULL);
}
