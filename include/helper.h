// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#ifndef _HELPER_H
#define _HELPER_H 

typedef enum {FALSE, TRUE} bool;
typedef uint64_t ticks;

int timed_memcpy(int *, const int *, int);
int timed_read(uint8_t *, int);
int mprintf(const char *, ...);
uint16_t get_smallest_idx(ticks *, unsigned);
uint16_t get_largest_idx(ticks *, unsigned);
uint32_t *get_rand_seq(unsigned, unsigned);

static inline void flush(volatile void *addr)
{
     asm volatile("clflush %0" : "+m" (*(volatile char *) addr));
}

#endif /* _HELPER_H */
