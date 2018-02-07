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
TARGET:= go
OBJ_DIR=obj$(PREFIX)
TOP:=$(PWD)

COMPILE=$(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -MD -c -o $@ $<
####################################################
LDFLAGS_GLOBAL += -L$(TOP)/lib
LDFLAGS_GLOBAL += -lssl -lcrypto -lm -ldl


CFLAGS_GLOBAL += -I$(TOP)/include 
CFLAGS_GLOBAL += -I$(TOP)/cjson
CFLAGS_GLOBAL += -Wall -Os  -D__WEB_DEBUG__
####################################################

####################################################
all: $(TARGET)

dir:=$(TOP)/goahead
include $(dir)/goahead.mk

dir:=$(TOP)/ssl
include $(dir)/ssl.mk

dir:=$(TOP)/cjson
include $(dir)/cjson.mk


$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c
	$(COMPILE)

OBJS:= $(OBJ_DIR)/web_main.o

$(TARGET): $(OBJ_DIR) $(OBJS) $(LIBS_LIST)
	$(CC) $(OBJS) $(LDFLAGS_PATH) $(LIBS_LIST) $(LDFLAGS_GLOBAL) -o $@
	$(STRIP) $@

CLEAN_LIST += $(OBJS)
CLEAN_LIST += $(OBJ_DIR:.o=.d)

clean:
	rm -rf $(CLEAN_LIST)
	rm -rf $(OBJ_DIR)
