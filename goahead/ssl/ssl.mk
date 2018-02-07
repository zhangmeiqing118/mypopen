#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_SSL := $(OBJ_DIR)/libopenssl.a

#SSL_FILES=`ls $d`
#define list_zebra_file
#@for file in $(SSL_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBSSL_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBSSL_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBSSL_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBSSL_DEPS_$(d)   :=  $(LIBSSL_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_SSL)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBSSL_OBJ_$(d)) $(LIBSSL_DEPS_$(d)) $(LIBRARY_SSL)

-include $(LIBSSL_DEPS_$(d))

$(LIBRARY_SSL): $(LIBSSL_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
