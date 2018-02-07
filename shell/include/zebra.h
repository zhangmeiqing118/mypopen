/* Zebra common header.
   Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _ZEBRA_H
#define _ZEBRA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <stdarg.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/utsname.h>

#include <sys/sysctl.h>

#include <sys/socket.h>
#include <arpa/inet.h> 
#include <arpa/telnet.h>
#include <net/if.h>
#include <netinet/tcp.h>

#include "config.h"

///zebra header
#include "log.h"
#include "thread.h"
#include "memory.h"
#include "buffer.h"
#include "vty.h"
#include "command.h"
#include "version.h"

#define MAXPATHLEN          1024
#define ZEBRA_ROUTE_MAX     9

#define AFI_IP              1
#define AFI_IP6             2

#define CHECK_FLAG(V,F)     ((V) & (F))

#endif
