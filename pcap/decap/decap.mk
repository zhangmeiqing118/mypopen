#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_DECAP := $(OBJ_DIR)/libdecap.a

#DECAP_FILES=`ls $d`
#define list_decap_file
#@for file in $(DECAP_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBDECAP_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBDECAP_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBDECAP_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBDECAP_DEPS_$(d)   :=  $(LIBDECAP_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_DECAP)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBDECAP_OBJ_$(d)) $(LIBDECAP_DEPS_$(d)) $(LIBRARY_DECAP)

-include $(LIBDECAP_DEPS_$(d))

$(LIBRARY_DECAP): $(LIBDECAP_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
