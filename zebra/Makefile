####################################################
#CROSS_COMPILE=powerpc-linux-gnu-
#CROSS_COMPILE=mips64-octeon-linux-gnu-

CC:=$(CROSS_COMPILE)gcc
AR:=$(CROSS_COMPILE)ar
STRIP:=$(CROSS_COMPILE)strip
####################################################
TARGET:= zebra0
OBJ_DIR=obj$(PREFIX)
TOP:=$(PWD)

COMPILE=$(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -MD -c -o $@ $<
####################################################
LDFLAGS_GLOBAL+= -lcrypt

CFLAGS_GLOBAL += -I$(TOP) -I$(TOP)/include
CFLAGS_GLOBAL += -I$(TOP)/comm  -I$(TOP)/zebra
CFLAGS_GLOBAL += -DHAVE_CONFIG_H -DMULTIPATH_NUM=1
####################################################

####################################################
all: $(TARGET)

dir:=$(TOP)/zebra
include $(dir)/zebra.mk

dir:=$(TOP)/comm
include $(dir)/comm.mk

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
