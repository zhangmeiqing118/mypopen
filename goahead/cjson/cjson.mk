#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_CJSON := $(OBJ_DIR)/libcjson.a

#CJSON_FILES=`ls $d`
#define list_zebra_file
#@for file in $(CJSON_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBCJSON_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBCJSON_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBCJSON_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBCJSON_DEPS_$(d)   :=  $(LIBCJSON_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_CJSON)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBCJSON_OBJ_$(d)) $(LIBCJSON_DEPS_$(d)) $(LIBRARY_CJSON)

-include $(LIBCJSON_DEPS_$(d))

$(LIBRARY_CJSON): $(LIBCJSON_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
