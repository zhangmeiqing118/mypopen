#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_TABLE := $(OBJ_DIR)/libtable.a

LIBTABLE_OBJ_$(d)  := $(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 
#LIBTABLE_OBJ_$(d)  :=  \
#	$(OBJ_DIR)/aclk_sdk_table.o


$(LIBTABLE_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall -Werror -Wundef -Wno-unused-parameter -Wundef -G0

#  standard component Makefile rules

LIBTABLE_DEPS_$(d)   :=  $(LIBTABLE_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_TABLE)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBTABLE_OBJ_$(d)) $(LIBTABLE_DEPS_$(d)) $(LIBRARY_TABLE)

-include $(LIBTABLE_DEPS_$(d))

$(LIBRARY_TABLE): $(LIBTABLE_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
