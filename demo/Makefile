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
TARGET:= cli
OBJ_DIR=obj$(PREFIX)
TOP:=$(PWD)

COMPILE=$(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -MD -c -o $@ $<
####################################################
LDFLAGS_GLOBAL += -L$(TOP)/lib/$(HOST)
LDFLAGS_GLOBAL += -levent -lzlog
LDFLAGS_GLOBAL += -lcrypt -lrt  -lm -lpthread 

CFLAGS_GLOBAL += -I$(TOP)/include 
CFLAGS_GLOBAL += -I$(TOP)/lib/include 
CFLAGS_GLOBAL += -I$(TOP)/comm 
CFLAGS_GLOBAL += -I$(TOP)/zebra
CFLAGS_GLOBAL += -Wall -Os #-DHAVE_IPV6
####################################################

####################################################
all: $(TARGET)

dir:=$(TOP)/comm
include $(dir)/comm.mk

dir:=$(TOP)/zebra
include $(dir)/zebra.mk

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c
	$(COMPILE)

OBJS:=$(OBJ_DIR)/main.o

$(TARGET): $(OBJ_DIR) $(OBJS) $(LIBS_LIST)
	$(CC) $(OBJS) $(LDFLAGS_PATH) $(LIBS_LIST) $(LDFLAGS_GLOBAL) -o $@
	$(STRIP) $@

CLEAN_LIST += $(OBJS)
CLEAN_LIST += $(OBJ_DIR:.o=.d)

clean:
	rm -rf $(CLEAN_LIST)
	rm -rf $(OBJ_DIR)
