TARGET=s0-counter
MCU=atmega328p
SOURCES=main.c uart.c fifo.c timer.c log.c proto.c i2c.c s0.c mem.c fram.c prefs.c
F_CPU=8000000

PROGRAMMER=stk500v2
PORT=-P/dev/ttyUSB0
BAUD=-B500kHz

CFLAGS=-flto -Os -Wall -Werror -std=c11 -fshort-enums -g2 -gdwarf -c -DF_CPU=$(F_CPU)UL -Wno-unused-function
LDFLAGS=-flto -Os -Wl,-Map,$(BINDIR)/$(TARGET).map -Wl,--section-start=.fram=0x860000

RM=rm
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude

SRCDIR=src
BINDIR=bin

OBJECTS=$(addprefix $(BINDIR)/, $(SOURCES:.c=.o))

DOXYGEN=doxygen
DOCDIR=doc

.PHONY: all size program doc clean

all: $(BINDIR)/$(TARGET).hex $(BINDIR)/$(TARGET).eep size

$(BINDIR)/$(TARGET).hex: $(BINDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -j .data -j .text $< $@

$(BINDIR)/$(TARGET).eep: $(BINDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -j .eeprom --change-section-lma .eeprom=1 $< $@

$(BINDIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $@

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -mmcu=$(MCU) $< -o $@

size: $(BINDIR)/$(TARGET).elf
	$(SIZE) --mcu=$(MCU) -C $<

program: $(BINDIR)/$(TARGET).hex
	$(AVRDUDE) -p $(MCU) $(PORT) $(BAUD) -c $(PROGRAMMER) -U flash:w:$<

doc:
	$(DOXYGEN) $(DOCDIR)/Doxyfile

clean:
	$(RM) -rf $(BINDIR)/*
	$(RM) -rf $(DOCDIR)/doxygen

