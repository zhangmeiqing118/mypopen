#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

dir := $(TOP)/lib/table
include $(dir)/table.mk

dir := $(TOP)/lib/shell
include $(dir)/shell.mk

dir := $(TOP)/lib/log
include $(dir)/log.mk

dir := $(TOP)/lib/uart
include $(dir)/uart.mk

dir := $(TOP)/lib/acsmx2
include $(dir)/acsmx2.mk

CFLAGS_GLOBAL += -I$(dir)/shell
CFLAGS_GLOBAL += -I$(dir)/log
CFLAGS_GLOBAL += -I$(dir)/uart
CFLAGS_GLOBAL += -I$(dir)/acsmx2

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
