##############################################################################
#   file: m16.mk                                                             #
#                                                                            #
#	  ABOS Bootloader                                                          #
#                                                                            #
#   This program is free software: you can redistribute it and/or modify     #
#   it under the terms of the GNU General Public License as published by     #
#   the Free Software Foundation, either version 3 of the License, or        #
#   (at your option) any later version.                                      #
#                                                                            #
#   This program is distributed in the hope that it will be useful,          #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#   GNU General Public License for more details.                             #
#                                                                            #
#   You should have received a copy of the GNU General Public License        #
#   along with this program.  If not, see <http://www.gnu.org/licenses/>     #
#                                                                            #
#   Written by Alfredo Orozco <alfredoopa@gmail.com>                         #
##############################################################################

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
