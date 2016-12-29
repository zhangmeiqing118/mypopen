#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_FLOW := $(OBJ_DIR)/libflow.a

#FLOW_FILES=`ls $d`
#define list_flow_file
#@for file in $(FLOW_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBFLOW_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBFLOW_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBFLOW_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBFLOW_DEPS_$(d)   :=  $(LIBFLOW_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_FLOW)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBFLOW_OBJ_$(d)) $(LIBFLOW_DEPS_$(d)) $(LIBRARY_FLOW)

-include $(LIBFLOW_DEPS_$(d))

$(LIBRARY_FLOW): $(LIBFLOW_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
