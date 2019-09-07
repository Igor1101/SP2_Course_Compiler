TARGET := app
PLATFORM ?= posix 
CC ?= gcc
# path
APP_PATH = $(abspath ./)
LAB1_PATH = $(APP_PATH)/LAB1
# main src file
APP_SRC += main_lab1.c
# app sources
APP_SRC += lab1.c
APP_SRC += utf8.c
# app includes
INCFLAGS +=-I$(APP_PATH)
# rtos includes
INCFLAGS += -I$(RTOS_PATH)
INCFLAGS += -I$(RTOS_INCLUDES_PATH)
CFLAGS = -c -g -Wall -DDEBUG_APP

# build
COBJS = $(patsubst %.c,%.o,$(APP_SRC))
# deps rules
.PHONY: all clean
# Path to directories containing application source 
vpath %.c $(APP_PATH):$(LAB1_PATH)


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
