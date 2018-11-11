#####################################################
#                  ABOS Makefile                    #
#			      (AVR Bootloader Open Source)            #
#                                                   #
# Makefile to create a ABOS bootloader application  #
#                                                   #
# AVR project create with APOS                      #
#####################################################

include devices/m128a.mk

PROJECT_NAME = abos
AVRDUDE_PROG = -c usbtiny
AVRDUDE_OPS  = 
AVRDUDE      = avrdude -p $(AVRDUDE_DEVICE) $(AVRDUDE_PROG) $(AVRDUDE_OPS) 
FUSES        = -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m -U lock:w:$(LOCK_BITS):m # -U efuse:w:$(EFUSE):m

OBJECT_FILES = abos.o

INCLUDEPATHS =  -I .

CFLAGS =  -g -Wall -Os -DDEVICE=$(DEVICE) -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) $(INCLUDEPATHS) -DFLASH_SIZE=$(FLASH_SIZE) -DBOOT_SIZE=$(BOOT_SIZE)
LDFLAGS = -g -Wall -Os -Wl,--section-start=.text=$(BOOT_START) -Wl,-Map,$(PROJECT_NAME).map

COMPILE = avr-gcc $(CFLAGS) 

# symbolic targets:
all: $(PROJECT_NAME).hex

.c.o:
	$(COMPILE) -c $< -o $@

.cpp.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

.cpp.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:$(PROJECT_NAME).hex:i

fuse:
	$(AVRDUDE) -V $(FUSES)

lfuse:
	$(AVRDUDE) -U lfuse:w:$(LFUSE):m

hfuse:
	$(AVRDUDE) -U hfuse:w:$(HFUSE):m

efuse:
	$(AVRDUDE) -U efuse:w:$(EFUSE):m

lockbits: 
	$(AVRDUDE) -U lock:w:$(LOCK_BITS):m

read_lockbits:
	$(AVRDUDE) -U lock:r:$(PROJECT_NAME).lockbits.bin:r

erase:
	$(AVRDUDE) -e

reset:
	$(AVRDUDE)

eeprom:	$(PROJECT_NAME).eep
	$(AVRDUDE) -U eeprom:w:$(PROJECT_NAME).eep:i

read_eeprom:
	$(AVRDUDE) -U eeprom:r:$(PROJECT_NAME).eeprom.bin:r

read_flash:
	$(AVRDUDE) -U flash:r:$(PROJECT_NAME).flash.bin:r
install: flash fuse

clean:
	rm -f $(PROJECT_NAME).hex $(PROJECT_NAME).elf $(PROJECT_NAME).eep $(PROJECT_NAME).map $(PROJECT_NAME).bin $(OBJECT_FILES) $(PROJECT_NAME).lock.hex $(PROJECT_NAME).lock

$(PROJECT_NAME).elf: $(OBJECT_FILES)
	avr-gcc $(LDFLAGS) -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) $(INCLUDEPATHS)  -o $(PROJECT_NAME).elf $(OBJECT_FILES) 

$(PROJECT_NAME).hex: $(PROJECT_NAME).elf
	rm -f $(PROJECT_NAME).hex
	avr-objcopy -j .text -j .data -O ihex $(PROJECT_NAME).elf $(PROJECT_NAME).hex
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $(PROJECT_NAME).elf $(PROJECT_NAME).eep
	avr-size --format=avr --mcu=$(DEVICE) $(PROJECT_NAME).elf

disasm:	$(PROJECT_NAME).elf
	avr-objdump -d $(PROJECT_NAME).elf

cpp:
	$(COMPILE) -E main.c
