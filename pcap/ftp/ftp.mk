#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_FRAG := $(OBJ_DIR)/libfrag.a

LIBFRAG_OBJ_$(d)  := $(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 
#LIBFRAG_OBJ_$(d)  :=  \
#	$(OBJ_DIR)/aclk_dpi_module.o	\
#	$(OBJ_DIR)/aclk_dpi_frag.o		\
#	$(OBJ_DIR)/aclk_dpi_connect.o	\
#	$(OBJ_DIR)/aclk_dpi_command.o


$(LIBFRAG_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall -Werror -Wundef -Wno-unused-parameter -Wundef -G0

#  standard component Makefile rules

LIBFRAG_DEPS_$(d)   :=  $(LIBFRAG_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_FRAG)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBFRAG_OBJ_$(d)) $(LIBFRAG_DEPS_$(d)) $(LIBRARY_FRAG)

-include $(LIBFRAG_DEPS_$(d))

$(LIBRARY_FRAG): $(LIBFRAG_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
