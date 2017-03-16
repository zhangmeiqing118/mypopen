#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_ACSMX2 := $(OBJ_DIR)/libacsmx2.a

LIBACSMX2_OBJ_$(d)  := $(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 
#LIBACSMX2_OBJ_$(d)  :=  \
#	$(OBJ_DIR)/aclk_sdk_acsmx2.o


$(LIBACSMX2_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall -Werror -Wundef -Wno-unused-parameter -Wundef -G0

#  standard component Makefile rules

LIBACSMX2_DEPS_$(d)   :=  $(LIBACSMX2_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_ACSMX2)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBACSMX2_OBJ_$(d)) $(LIBACSMX2_DEPS_$(d)) $(LIBRARY_ACSMX2)

-include $(LIBACSMX2_DEPS_$(d))

$(LIBRARY_ACSMX2): $(LIBACSMX2_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
