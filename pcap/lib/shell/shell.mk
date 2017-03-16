#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_SHELL := $(OBJ_DIR)/libshell.a

LIBSHELL_OBJ_$(d)  := $(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 
#LIBSHELL_OBJ_$(d)  :=  \
#	$(OBJ_DIR)/aclk_sdk_shell.o


$(LIBSHELL_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall -Werror -Wundef -Wno-unused-parameter -Wundef -G0

#  standard component Makefile rules

LIBSHELL_DEPS_$(d)   :=  $(LIBSHELL_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_SHELL)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBSHELL_OBJ_$(d)) $(LIBSHELL_DEPS_$(d)) $(LIBRARY_SHELL)

-include $(LIBSHELL_DEPS_$(d))

$(LIBRARY_SHELL): $(LIBSHELL_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
