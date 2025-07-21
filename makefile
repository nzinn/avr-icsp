

CC = avr-gcc -Wall -std=c99 

CFLAGS = -mmcu=atmega328p -O1

OBJFILES = main.o spi_base.o iohelper.o usart.o spi_instructions.o packet.o prog_commands.o


# Name of the elf file
NAME = SPI_Test

$(NAME).elf: $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $(NAME).elf

%.o: ./src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

install:
	avrdude -c arduino -p m328p -P /dev/ttyUSB0 -e -U flash:w:$(NAME).elf
