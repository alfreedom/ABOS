##################################################
#       Params or ATmega16 Microcontroller       #
##################################################
DEVICE         = atmega16
AVRDUDE_DEVICE = atmega16
CLOCK          = 16000000
LFUSE          = 0xFF
HFUSE          = 0xDA
EFUSE          = 0xFF
LOCK_BITS      = 0xEF
FLASH_SIZE     = 16384L # 16k * 1024
BOOT_SIZE      = 512
BOOT_START     = 0x3C00 # 0x1E00 * 2
