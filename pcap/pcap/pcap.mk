#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_PCAP := $(OBJ_DIR)/libpcap.a

#PCAP_FILES=`ls $d`
#define list_pcap_file
#@for file in $(PCAP_FILES);							\
#	do 												\
#	( echo "$(CC) -c $$file" && $(CC) -c $$file ) 	\
#	done;
#endef
#
#%.o: %.c
#	$(call compile_c_file)

LIBPCAP_OBJ_$(d)  :=$(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 

#LIBPCAP_OBJ_$(d)  :=  \
	$(OBJ_DIR)/test0.o


$(LIBPCAP_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall 
	#-Werror -Wundef -Wno-unused-parameter -Wundef

#  standard component Makefile rules

LIBPCAP_DEPS_$(d)   :=  $(LIBPCAP_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_PCAP)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBPCAP_OBJ_$(d)) $(LIBPCAP_DEPS_$(d)) $(LIBRARY_PCAP)

-include $(LIBPCAP_DEPS_$(d))

$(LIBRARY_PCAP): $(LIBPCAP_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
