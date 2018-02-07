/* Generic vector interface routine
 * Copyright (C) 1997 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

/* Initialize vector : allocate memory and return vector. */
vector vector_init(unsigned int size)
{
	vector v;
    
    v = malloc(sizeof(struct _vector));
    if (NULL == v) {
        printf("malloc for vector failed\n");
        return NULL;
    }
    memset(v, 0x00, sizeof(struct _vector));

    if (!size) {
        size = 1;
    }
    v->index = malloc(sizeof(void *) * size);
    if (NULL == v->index) {
        printf("malloc for v index failed\n");
        free(v);
        v = NULL;
        return NULL;
    }
    memset(v->index, 0x00, sizeof(void *) * size);
    v->max = 0;
    v->alloced = size;

	return v;
}

void vector_free(vector v)
{
    int i;

	if (v) {
        if (v->index) {
            for (i = 0; i < v->max; i++) {
                if (v->index[i]) {
                    free(v->index[i]);
                    v->index[i] = NULL;
                }
            }
            free(v->index);
            v->index = NULL;
        }
        free(v);
        v = NULL;
    }
}

vector vector_copy(vector v)
{
	unsigned int size;
	vector new;
    
    new = malloc(sizeof(struct _vector));
    if (NULL == new) {
        printf("malloc for copy vector failed\n");
        return NULL;
    }

	new->max = v->max;
	new->alloced = v->alloced;
	size = sizeof(void *) * (v->alloced);
	new->index = malloc(size);
    if (NULL == new->index) {
        printf("malloc for copy vector index failed\n");
        free(new);
        new = NULL;
        return NULL;
    }
	memcpy(new->index, v->index, size);

	return new;
}

/* Check assigned index, and if it runs short double index pointer */
int vector_ensure(vector v, unsigned int num)
{
    void *ptr;
    unsigned int alloced;

	if (v->alloced > num) {
		return 0;
    }

    alloced = v->alloced;
    while (v->alloced < num) {
        v->alloced *= 2;
    }
	ptr = realloc(v->index, sizeof(void *) * (v->alloced));
    if (NULL == ptr) {
        printf("realloc for ensure vector index failed\n");
        return -1;
    }
    v->index = ptr;
	memset(&v->index[alloced], 0, sizeof(void *) * (v->alloced - alloced));

    return 0;
}

/* This function only returns next empty slot index.  It dose not mean
   the slot's index memory is assigned, please call vector_ensure()
   after calling this function. */
int vector_empty_slot(vector v)
{
	unsigned int i;

	if (v->max == 0) {
		return 0;
    }

	for (i = 0; i < v->max; i++) {
		if (NULL == v->index[i]) {
			return i;
        }
    }

	return i;
}

/* Set value to the smallest empty slot. */
int vector_set(vector v, void *val)
{
    int recode;
	unsigned int i;

	i = vector_empty_slot(v);
	recode = vector_ensure(v, i);
    if (recode) {
        return -1;
    }

	v->index[i] = val;
	if (v->max <= i) {
		v->max = i + 1;
    }

	return i;
}

/* Set value to specified index slot. */
int vector_set_index(vector v, unsigned int i, void *val)
{
    int recode;

	recode = vector_ensure(v, i);
    if (recode) {
        return -1;
    }
	v->index[i] = val;
	if (v->max <= i) {
		v->max = i + 1;
    }

	return 0;
}

/* Look up vector.  */
void *vector_lookup(vector v, unsigned int i)
{
	if (i >= v->max) {
		return NULL;
    }

	return v->index[i];
}

/* Lookup vector, ensure it. */
void *vector_lookup_ensure(vector v, unsigned int i)
{
    int recode;

	recode = vector_ensure(v, i);
    if (recode) {
        return NULL;
    }

	return v->index[i];
}

#if 0
/* Unset value at specified index slot. */
void vector_unset(vector v, unsigned int i)
{
	if (i >= v->alloced) {
		return;
    }

	v->index[i] = NULL;
	if (i + 1 == v->max) {
		v->max--;
		while (i && v->index[--i] == NULL && v->max--) ;	/* Is this ugly ? */
	}
}
#endif

/* Count the number of not emplty slot. */
unsigned int vector_count(vector v)
{
	unsigned int i;
	unsigned count = 0;

	for (i = 0; i < v->max; i++) {
		if (v->index[i] != NULL) {
			count++;
        }
    }

	return count;
}
