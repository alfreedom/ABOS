##################################################
#       Params or ATmega32 Microcontroller       #
##################################################
DEVICE         = atmega32
AVRDUDE_DEVICE = atmega32
CLOCK          = 16000000
LFUSE          = 0xFF
HFUSE          = 0x9C
EFUSE          = 0xFF
LOCK_BITS      = 0xEF
FLASH_SIZE     = 32768L # 32k * 1024
BOOT_SIZE      = 512
BOOT_START     = 0x7C00 # 0x3E00 * 2