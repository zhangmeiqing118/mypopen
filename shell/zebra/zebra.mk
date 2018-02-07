#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_ZEBRA := $(OBJ_DIR)/libzebra.a

#ZEBRA_FILES=`ls $d`
#define list_zebra_file
#@for file in $(ZEBRA_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBZEBRA_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBZEBRA_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBZEBRA_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBZEBRA_DEPS_$(d)   :=  $(LIBZEBRA_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_ZEBRA)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBZEBRA_OBJ_$(d)) $(LIBZEBRA_DEPS_$(d)) $(LIBRARY_ZEBRA)

-include $(LIBZEBRA_DEPS_$(d))

$(LIBRARY_ZEBRA): $(LIBZEBRA_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
