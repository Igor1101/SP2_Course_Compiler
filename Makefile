TARGET := app
PLATFORM ?= posix 
CC ?= gcc
LANGUAGE_ ?=c
# path
APP_PATH = $(realpath ./)
LAB1_PATH = $(APP_PATH)/lexems
SYN_PATH = $(APP_PATH)/syntax
SEM_PATH = $(APP_PATH)/semantic
GEN_PATH = $(APP_PATH)/gen
# main src file
APP_SRC += main.c
# app sources
APP_SRC += ch_utf8.c
APP_SRC += utf8.c
APP_SRC += cli.c
APP_SRC += tables.c
APP_SRC += MooreMachine.c
APP_SRC += lex_$(LANGUAGE_).c
APP_SRC += lex_utils.c
APP_SRC += syntax_$(LANGUAGE_).c
APP_SRC += syn_utils.c
APP_SRC += sem_$(LANGUAGE_).c
APP_SRC += tables_sem.c
APP_SRC += sem_utils.c
APP_SRC += prelim.c
APP_SRC += vm.c
# app includes
INCFLAGS +=-I$(realpath $(APP_PATH))
# rtos includes
INCFLAGS += -I$(realpath $(RTOS_PATH))
INCFLAGS += -I$(realpath $(RTOS_INCLUDES_PATH))
CFLAGS = -c -g -Wall -DDEBUG_APP

# build
COBJS = $(patsubst %.c,%.o,$(APP_SRC))
# deps rules
.PHONY: all clean
# Path to directories containing application source 
vpath %.c $(APP_PATH):$(LAB1_PATH):$(SYN_PATH):$(SEM_PATH):$(GEN_PATH):trees


all: $(APP_SRC) $(COBJS) $(TARGET).elf 



%.o: %.c $(BUILD_DIR) 
	$(CC) $(CFLAGS)  $(RTOS_DEFS) $(INCFLAGS) -c $< -o $@
$(TARGET).elf: $(COBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(INCFLAGS)
clean:
	-rm -rf *.o
	-rm -rf *.elf
$(BUILD_DIR):
	mkdir -p  $(BUILD_DIR)

run:
	./$(TARGET).elf
