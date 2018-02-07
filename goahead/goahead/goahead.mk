#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_GOAHEAD := $(OBJ_DIR)/libgo.a

#GOAHEAD_FILES=`ls $d`
#define list_zebra_file
#@for file in $(GOAHEAD_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBGOAHEAD_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBGOAHEAD_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBGOAHEAD_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBGOAHEAD_DEPS_$(d)   :=  $(LIBGOAHEAD_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_GOAHEAD)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBGOAHEAD_OBJ_$(d)) $(LIBGOAHEAD_DEPS_$(d)) $(LIBRARY_GOAHEAD)

-include $(LIBGOAHEAD_DEPS_$(d))

$(LIBRARY_GOAHEAD): $(LIBGOAHEAD_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
