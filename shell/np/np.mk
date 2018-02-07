#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_NP := $(OBJ_DIR)/libnp.a

#NP_FILES=`ls $d`
#define list_np_file
#@for file in $(NP_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBNP_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBNP_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBNP_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBNP_DEPS_$(d)   :=  $(LIBNP_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_NP)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBNP_OBJ_$(d)) $(LIBNP_DEPS_$(d)) $(LIBRARY_NP)

-include $(LIBNP_DEPS_$(d))

$(LIBRARY_NP): $(LIBNP_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
