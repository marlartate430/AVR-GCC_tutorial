PROGRAMAS=$(HOME)/Escritorio/programas
AVR=$(PROGRAMAS)/avr8-gnu-toolchain-linux_x86_64/bin
AVRDUDE_DIR=$(PROGRAMAS)/avrdude
MICRO_TARGET=atmega328p
FILENAME=pwm0
CC=$(AVR)/avr-gcc
CONF=$(AVRDUDE_DIR)/etc/avrdude.conf
PORT=/dev/ttyACM0
PART=m328p
AVRDUDE=$(AVRDUDE_DIR)/bin/avrdude
OBJCOPY=$(AVR)/avr-objcopy
INCLUDE=./include

load: $(FILENAME).hex
	sudo $(AVRDUDE) -c arduino -C $(CONF) -p $(PART) -P $(PORT) -U flash:w:$(FILENAME).hex:i
    
$(FILENAME).hex: $(FILENAME).elf
	$(OBJCOPY) -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex

$(FILENAME).elf: $(FILENAME).o
	$(CC) $(FILENAME).o -mmcu=$(MICRO_TARGET) -o $(FILENAME).elf

$(FILENAME).o: $(FILENAME).c
	$(CC) $(FILENAME).c -I$(INCLUDE) -mmcu=$(MICRO_TARGET) -o $(FILENAME).o -c

clean:
	rm -rf $(FILENAME).o
	rm -rf $(FILENAME).elf
	rm -rf $(FILENAME).hex
