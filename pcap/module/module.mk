#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_MODULE := $(OBJ_DIR)/libmodule.a

#MODULE_FILES=`ls $d`
#define list_module_file
#@for file in $(MODULE_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBMODULE_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBMODULE_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBMODULE_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBMODULE_DEPS_$(d)   :=  $(LIBMODULE_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_MODULE)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBMODULE_OBJ_$(d)) $(LIBMODULE_DEPS_$(d)) $(LIBRARY_MODULE)

-include $(LIBMODULE_DEPS_$(d))

$(LIBRARY_MODULE): $(LIBMODULE_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
