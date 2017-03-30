/*
 * Buffering of output and input. 
 * Copyright (C) 1998 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 * 
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include "zlog.h"
#include "buffer.h"

struct buffer_data *buffer_data_new(size_t size)
{
	struct buffer_data *d;

	d = malloc(sizeof(struct buffer_data));
    if (NULL == d) {
        printf("buffer malloc failed");
        return NULL;
    }
	memset(d, 0, sizeof(struct buffer_data));
	d->data = malloc(size);
    if (NULL == d->data) {
        printf("buffer data malloc failed");
        free(d);
        d = NULL;
        return NULL;
    }

	return d;
}

void buffer_data_free(struct buffer_data *d)
{
    if (d) {
        if (d->data) {
            free(d->data);
            d->data = NULL;
        }
        free(d);
        d = NULL;
    }

    return;
}

struct buffer *buffer_new(size_t size)
{
	struct buffer *b;

	b = malloc(sizeof(struct buffer));
    if (NULL == b) {
        printf("buff malloc failed");
        return NULL;
    }
	memset(b, 0, sizeof(struct buffer));
	b->size = size;

	return b;
}

void buffer_free(struct buffer *b)
{
	struct buffer_data *d;
	struct buffer_data *next;

    if (b) {
        d = b->head;
        while (d) {
            next = d->next;
            buffer_data_free(d);
            d = next;
        }

        d = b->unused_head;
        while (d) {
            next = d->next;
            buffer_data_free(d);
            d = next;
        }
        free(b);
        b = NULL;
    }

    return;
}

char *buffer_getstr(struct buffer *b)
{
    if (b && b->head)  {
        return strdup((char *)b->head->data);
    }

	return NULL;
}

int buffer_empty(struct buffer *b)
{
    if (NULL == b) {
        return 1;
    }
	if ((NULL == b->tail) || (b->tail->cp == b->tail->sp)) {
		return 1;
    }
	
    return 0;
}

void buffer_reset(struct buffer *b)
{
	struct buffer_data *data;
	struct buffer_data *next;

    if (b) {
        for (data = b->head; data; data = next) {
            next = data->next;
            buffer_data_free(data);
        }
        b->head = b->tail = NULL;
        b->alloc = 0;
        b->length = 0;
    }

    return;
}

int buffer_add(struct buffer *b)
{
	struct buffer_data *d;

    if (!b) {
        return -1;
    }

    d = buffer_data_new(b->size);
    if (b->tail == NULL) {
        d->prev = NULL;
        d->next = NULL;
        b->head = d;
        b->tail = d;
    } else {
        d->prev = b->tail;
        d->next = NULL;
        b->tail->next = d;
        b->tail = d;
    }
    b->alloc++;

    return 0;
}

int buffer_write(struct buffer *b, char *ptr, size_t size)
{
	struct buffer_data *data;

    if (!b) {
        return -1;
    }

	data = b->tail;
	b->length += size;
	while (size) {
		/* If there is no data buffer add it. */
		if (data == NULL || data->cp == b->size) {
			if (buffer_add(b)) {
                return -1;
            }
			data = b->tail;
		}

		/* Last data. */
		if (size <= (b->size - data->cp)) {
			memcpy((data->data + data->cp), ptr, size);
			data->cp += size;
			size = 0;
		} else {
			memcpy((data->data + data->cp), ptr, (b->size - data->cp));
			size -= (b->size - data->cp);
			ptr += (b->size - data->cp);
			data->cp = b->size;
		}
	}

	return 0;
}

int buffer_putc(struct buffer *b, char c)
{
    int recode;
	
    recode = buffer_write(b, &c, 1);
    if (recode) {
        return recode;
    }

	return 0;
}

int buffer_putw(struct buffer *b, short c)
{
    int recode;

	recode = buffer_write(b, (char *)&c, 2);
    if (recode) {
        return recode;
    }

	return 0;
}

int buffer_putstr(struct buffer *b, char * c)
{
	int recode;

	recode = buffer_write(b, c, strlen(c));
    if (recode) {
        return recode;
    }

	return 0;
}

int buffer_flush(struct buffer *b, int fd, size_t size)
{
	int iov_index;
	struct iovec *iovec;
	struct buffer_data *data;
	struct buffer_data *out;
	struct buffer_data *next;

    if (NULL == b) {
        return -1;
    }

	iov_index = 0;
	iovec = malloc(sizeof(struct iovec) * b->alloc);
    if (NULL == iovec) {
        printf("iovec malloc failed");
        return -1;
    }
    memset(iovec, 0x00, sizeof(struct iovec) * b->alloc);

	for (data = b->head; data; data = data->next) {
		iovec[iov_index].iov_base = (char *)(data->data + data->sp);

		if (size <= (data->cp - data->sp)) {
			iovec[iov_index++].iov_len = size;
			data->sp += size;
			if (data->sp == data->cp) {
				data = data->next;
            }
			break;
		} else {
			iovec[iov_index++].iov_len = data->cp - data->sp;
			size -= data->cp - data->sp;
			data->sp = data->cp;
		}
	}

	writev(fd, iovec, iov_index);
	for (out = b->head; out && out != data; out = next) {
		next = out->next;
		if (next) {
			next->prev = NULL;
        } else {
			b->tail = next;
        }
		b->head = next;

		buffer_data_free(out);
		b->alloc--;
	}
	free(iovec);

    return 0;
}

int buffer_flush_all(struct buffer *b, int fd)
{
	int recode;
	struct buffer_data *d;
	int iov_index;
	struct iovec *iovec;

	if (buffer_empty(b)) {
		return 0;
    }

	iovec = malloc(sizeof(struct iovec) * b->alloc);
    if (NULL == iovec) {
        printf("iovec malloc failed");
        return -1;
    }
    memset(iovec, 0x00, sizeof(struct iovec) * b->alloc);
	iov_index = 0;

	for (d = b->head; d; d = d->next) {
		iovec[iov_index].iov_base = (char *)(d->data + d->sp);
		iovec[iov_index].iov_len = d->cp - d->sp;
		iov_index++;
	}
	recode = writev(fd, iovec, iov_index);
    if (-1 == recode) {
        free(iovec);
        buffer_reset(b);
        printf("iovec write failed");
        return -1;
    }
	free(iovec);
	buffer_reset(b);

	return 0;
}

int buffer_flush_vty_all(struct buffer *b, int fd, int erase_flag, int no_more_flag)
{
	int nbytes;
	int iov_index;
	struct iovec *iov;
	struct iovec small_iov[3];
	char more[] = " --More-- ";
	char erase[] =
	    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
	};
	struct buffer_data *data;
	struct buffer_data *out;
	struct buffer_data *next;

    if (NULL == b) {
        printf("Invalid buffer\n");
        return -1;
    }
	/* For erase and more data add two to b's buffer_data count. */
	if (b->alloc == 1) {
		iov = small_iov;
    } else {
		iov = malloc(sizeof(struct iovec) * (b->alloc + 2));
        if (NULL == iov) {
            printf("Malloc iov buffer failed\n");
            return -1;
        }
    }

	data = b->head;
	iov_index = 0;
	if (erase_flag) {
		iov[iov_index].iov_base = erase;
		iov[iov_index].iov_len = sizeof(erase);
		iov_index++;
	}

	/* Output data. */
	for (data = b->head; data; data = data->next) {
		iov[iov_index].iov_base = (char *)(data->data + data->sp);
		iov[iov_index].iov_len = data->cp - data->sp;
		iov_index++;
	}

	/* In case of `more' display need. */
	if (!buffer_empty(b) && !no_more_flag) {
		iov[iov_index].iov_base = more;
		iov[iov_index].iov_len = sizeof more;
		iov_index++;
	}

	/* We use write or writev */
	nbytes = writev(fd, iov, iov_index);

	/* Error treatment. */
	///if (nbytes < 0) {
	///	if (errno == EINTR) ;
	///	if (errno == EWOULDBLOCK) ;
	///}

	/* Free printed buffer data. */
	for (out = b->head; out && out != data; out = next) {
		next = out->next;
		if (next)
			next->prev = NULL;
		else
			b->tail = next;
		b->head = next;

		buffer_data_free(out);
		b->alloc--;
	}

	if (iov != small_iov) {
		free(iov);
    }

	return nbytes;
}

int buffer_flush_vty(struct buffer *b, int fd, int size, int erase_flag, int no_more_flag)
{
	int nbytes;
	int iov_index;
	struct iovec *iov;
	struct iovec small_iov[3];
	char more[] = " --More-- ";
	char erase[] =
	    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
	};
	struct buffer_data *data;
	struct buffer_data *out;
	struct buffer_data *next;

#ifdef  IOV_MAX
	int iov_size;
	int total_size;
	struct iovec *c_iov;
	int c_nbytes;
#endif /* IOV_MAX */

	/* For erase and more data add two to b's buffer_data count. */
	if (b->alloc == 1) {
		iov = small_iov;
    } else {
		iov = malloc(sizeof(struct iovec) * (b->alloc + 2));
        if (NULL == iov) {
            return -1;
        }
    }

	data = b->head;
	iov_index = 0;

	/* Previously print out is performed. */
	if (erase_flag) {
		iov[iov_index].iov_base = erase;
		iov[iov_index].iov_len = sizeof(erase);
		iov_index++;
	}

	/* Output data. */
	for (data = b->head; data; data = data->next) {
		iov[iov_index].iov_base = (char *)(data->data + data->sp);

		if (size <= (int)(data->cp - data->sp)) {
			iov[iov_index++].iov_len = size;
			data->sp += size;
			if (data->sp == data->cp)
				data = data->next;
			break;
		} else {
			iov[iov_index++].iov_len = data->cp - data->sp;
			size -= (data->cp - data->sp);
			data->sp = data->cp;
		}
	}

	/* In case of `more' display need. */
	if (!buffer_empty(b) && !no_more_flag) {
		iov[iov_index].iov_base = more;
		iov[iov_index].iov_len = sizeof(more);
		iov_index++;
	}

	/* We use write or writev */

#ifdef IOV_MAX
	/* IOV_MAX are normally defined in <sys/uio.h> , Posix.1g.
	   example: Solaris2.6 are defined IOV_MAX size at 16.     */
	c_iov = iov;
	total_size = iov_index;
	nbytes = 0;

	while (total_size > 0) {
		/* initialize write vector size at once */
		iov_size = (total_size > IOV_MAX) ? IOV_MAX : total_size;

		c_nbytes = writev(fd, c_iov, iov_size);

		if (c_nbytes < 0) {
			///if (errno == EINTR) ;
			///;
			///if (errno == EWOULDBLOCK) ;
			///;
			nbytes = c_nbytes;
			break;

		}

		nbytes += c_nbytes;

		/* move pointer io-vector */
		c_iov += iov_size;
		total_size -= iov_size;
	}
#else /* IOV_MAX */
	nbytes = writev(fd, iov, iov_index);

	/* Error treatment. */
	//if (nbytes < 0) {
	//	if (errno == EINTR) ;
	//	if (errno == EWOULDBLOCK) ;
	//}
#endif /* IOV_MAX */

	/* Free printed buffer data. */
	for (out = b->head; out && out != data; out = next) {
		next = out->next;
		if (next)
			next->prev = NULL;
		else
			b->tail = next;
		b->head = next;

		buffer_data_free(out);
		b->alloc--;
	}

	if (iov != small_iov) {
		free(iov);
    }

	return nbytes;
}

int buffer_flush_window(struct buffer *b, int fd, int width, int height, int erase, int no_more)
{
	unsigned long cp;
	unsigned long size;
	int lp;
	int lineno;
	struct buffer_data *data;

	if (height >= 2) {
		height--;
    }

	lp = 0;
	lineno = 0;
	size = 0;
	for (data = b->head; data; data = data->next) {
		cp = data->sp;

		while (cp < data->cp) {
			if (data->data[cp] == '\n' || lp == width) {
				lineno++;
				if (lineno == height) {
					cp++;
					size++;
					buffer_flush_vty(b, fd, size, erase, no_more);
                    return 0;
				}
				lp = 0;
			}
			cp++;
			lp++;
			size++;
		}
	}
    buffer_flush_vty(b, fd, size, erase, no_more);

	return 0;
}
