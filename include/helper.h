// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Carles Pey <cpey@pm.me>
 */

#ifndef _HELPER_H
#define _HELPER_H 

typedef enum {FALSE, TRUE} bool;

void flush(void *);
int timed_memcpy(int *, const int *, int);
int mprintf(const char *, ...);

#endif /* _HELPER_H */
