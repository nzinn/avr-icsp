

CC = avr-gcc -Wall -std=c99 

CFLAGS = -mmcu=atmega328p -O1

OBJ_NAMES = main.o spi_base.o iohelper.o usart.o spi_instructions.o packet.o prog_commands.o

SRC_DIR := src
BUILD_DIR := build


OBJFILES = $(addprefix $(BUILD_DIR)/,$(OBJ_NAMES))

# Name of the elf file
NAME = SPI_PROG

$(NAME).elf: $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $(NAME).elf

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | build
	$(CC) -c $(CFLAGS) $< -o $@

build:
	mkdir -p $(BUILD_DIR)

install:
	avrdude -c arduino -p m328p -P /dev/ttyUSB0 -e -U flash:w:$(NAME).elf
hex:
	avr-objcopy -O ihex $(NAME).elf $(NAME).hex





TEST_DIR := test
# TEST_SRC := $(wildcard $(TEST_DIR)/*/*.c)
TEST_BUILD_DIR := $(TEST_DIR)/build




# Find test source files like test/test_foo/test_foo.c
TEST_SRC := $(wildcard $(TEST_DIR)/test_*/test_*.c)

# Convert to binary names like test/build/test_foo.bin
TEST_NAMES := $(notdir $(basename $(TEST_SRC)))
TEST_BIN_ALL := $(addprefix $(TEST_BUILD_DIR)/,$(addsuffix .bin,$(TEST_NAMES)))

# TEST_BIN_ALL := $(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%.bin,$(TEST_SRC))
EXCLUDE = $(TEST_BUILD_DIR)/unity.bin
TEST_BIN := $(filter-out $(EXCLUDE), $(TEST_BIN_ALL))


TEST_CC = gcc 
TEST_CFLAGS = -O1 -std=c99


.PHONY: test test_build

test: $(TEST_BIN) | test_build
	./$(TEST_DIR)/run_tests.sh


$(TEST_BUILD_DIR)/test_%.bin: $(TEST_BUILD_DIR)/test_%.o $(TEST_BUILD_DIR)/%.o unity.o
	$(TEST_CC) $(TEST_CFLAGS) $^ -o $@




# Test harnesses
.SECONDEXPANSION:
$(TEST_BUILD_DIR)/test_%.o: $(TEST_DIR)/test_$$(*F)/test_$$(*F).c | test_build 
	$(TEST_CC) $(TEST_CFLAGS) -c $< -o $@



$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/test_$$(*F)/$$(*F).c | test_build 
	$(TEST_CC) $(TEST_CFLAGS) -I $(TEST_DIR)/test_$(*F) -c $< -o $@



$(TEST_DIR)/%.c: $(SRC_DIR)/$$(notdir %.c)
	cp $< $@ && cp $(SRC_DIR)/$(*F).h $(TEST_DIR)/$(*D)/$(*F).h

unity.o: $(TEST_DIR)/unity.c
	$(TEST_CC) $(TEST_CFLAGS) -c $< -o $@

test_build:
	mkdir -p $(TEST_BUILD_DIR)




