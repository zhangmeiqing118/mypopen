####################################################
#HOST = powerpc
#HOST = mips
HOST = x64

ifeq ($(HOST), powerpc)
    CROSS=powerpc-linux-gnu-
endif
ifeq ($(HOST), mips)
    CROSS=mips64-octeon-linux-gnu-
endif

CC = $(CROSS)gcc
AR = $(CROSS)ar
LD = $(CROSS)ld
STRIP = $(CROSS)strip
OBJDUMP = $(CROSS)objdump
NM = $(CROSS)num
####################################################
TOP:=$(PWD)

COMPILE=$(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -MD -c -o $@ $<
####################################################

CFLAGS_GLOBAL += -Wall -Werror -Os
####################################################

####################################################
SRCS_LIST := $(wildcard *.c) $(wildcard *.cc) 
OBJS_LIST := $(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SRCS_LIST))) 
#SRCS_LIST := $(wildcard *.c) $(wildcard *.cc) 
#OBJS_LIST := $(patsubst %.c,%.o, $(SRCS_LIST)) 
DEPS_LIST := $(patsubst %.o,%.d,$(OBJS_LIST)) 

#EXEC_LIST := passwd keyboard keyboard1
#EXEC_LIST := $(subst .o, , $(OBJS_LIST))
#EXEC_LIST := $(basename  $(OBJS_LIST))
EXEC_LIST := $(patsubst %.o,%,$(OBJS_LIST))

all:$(EXEC_LIST)

every:$(EXEC_LIST)

%.o:%.c
	$(COMPILE)

deps : $(DEPS_LIST)

objs: $(OBJS_LIST)

exec:$(EXEC_LIST)

-include $(DEPS_LIST) 



#$(EXEC_LIST): $(OBJS_LIST)
#	CMD=$(foreach(name, $(EXEC_LIST), $(name).o))
#	echo $(CMD)

clean:
	rm -f $(OBJS_LIST) $(DEPS_LIST) $(EXEC_LIST)
