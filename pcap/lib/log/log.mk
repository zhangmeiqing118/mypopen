#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_LOG := $(OBJ_DIR)/liblog.a

LIBLOG_OBJ_$(d)  := $(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 
#LIBLOG_OBJ_$(d)  :=  \
#	$(OBJ_DIR)/aclk_sdk_log.o


$(LIBLOG_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall -Werror -Wundef -Wno-unused-parameter -Wundef -G0

#  standard component Makefile rules

LIBLOG_DEPS_$(d)   :=  $(LIBLOG_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_LOG)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBLOG_OBJ_$(d)) $(LIBLOG_DEPS_$(d)) $(LIBRARY_LOG)

-include $(LIBLOG_DEPS_$(d))

$(LIBRARY_LOG): $(LIBLOG_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
