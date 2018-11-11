##################################################
#      Params or ATmega128A Microcontroller      #
##################################################
DEVICE         = atmega128a
AVRDUDE_DEVICE = atmega128
CLOCK          = 16000000
LFUSE          = 0xFF
HFUSE          = 0xDC
EFUSE          = 0xFF
LOCK_BITS      = 0xEF
FLASH_SIZE     = 131072L # 128k * 1024
BOOT_SIZE      = 1024
BOOT_START     = 0x1F800 # 0xFC00 * 2
