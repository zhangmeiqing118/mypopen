#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_COMM := $(OBJ_DIR)/libcomm.a

#COMM_FILES=`ls $d`
#define list_comm_file
#@for file in $(COMM_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBCOMM_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBCOMM_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBCOMM_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBCOMM_DEPS_$(d)   :=  $(LIBCOMM_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_COMM)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBCOMM_OBJ_$(d)) $(LIBCOMM_DEPS_$(d)) $(LIBRARY_COMM)

-include $(LIBCOMM_DEPS_$(d))

$(LIBRARY_COMM): $(LIBCOMM_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
