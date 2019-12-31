##############################################################################
#   file: m128a.mk                                                           #
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
#      Params or ATmega128A Microcontroller      #
##################################################
DEVICE         = atmega128a
AVRDUDE_DEVICE = atmega128
MCU_MODEL	   = m128a
CLOCK          = 16000000
LFUSE          = 0xFF
HFUSE          = 0xDC
EFUSE          = 0xFF
LOCK_BITS      = 0xEF
FLASH_SIZE     = 131072L # 128k * 1024
BOOT_SIZE      = 1024
BOOT_START     = 0x1F800 # 0xFC00 * 2
