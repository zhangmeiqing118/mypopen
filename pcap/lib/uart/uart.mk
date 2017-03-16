#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY_UART := $(OBJ_DIR)/libuart.a

LIBUART_OBJ_$(d)  := $(patsubst $(d)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(d)/*.c)) 
#LIBUART_OBJ_$(d)  :=  \
#	$(OBJ_DIR)/aclk_sdk_uart.o


$(LIBUART_OBJ_$(d)):  CFLAGS_LOCAL := -I$(d) -O2 -g -W -Wall -Werror -Wundef -Wno-unused-parameter -Wundef -G0

#  standard component Makefile rules

LIBUART_DEPS_$(d)   :=  $(LIBUART_OBJ_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY_UART)

CLEAN_LIST  :=  $(CLEAN_LIST) $(LIBUART_OBJ_$(d)) $(LIBUART_DEPS_$(d)) $(LIBRARY_UART)

-include $(LIBUART_DEPS_$(d))

$(LIBRARY_UART): $(LIBUART_OBJ_$(d))
	$(AR) -cr $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))
