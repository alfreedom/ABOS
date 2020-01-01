
# ABOS (AVR Bootloader Open Source)

### Simple bootloader to avoid ISP hardware to program AVR Microcontrollers


## Supported Devices
- ATmega16
- ATmega32
- ATmega128/ATmega128A


### The default baudrate is **_38400_**, is defined in the firs code line of the **_"abos.c"_** file. If you need other baudrate, change these value.
---

# Installation

1. Download the repository:

        git clone https://github.com/alfreedom/abos

2. Edit the include device configuration in the Makefile to choose the microcontroller:
        
        include devices/myDevice.mk

3. Change the **AVRDUDE_PROG** to specify your programmer in the Makefile:

        AVRDUDE_PROG = -c usbtiny

4. Enter to the bootloader folder and run:

        make all
        make install

        or

        make
        make flash
        make fuse
        make lockbits

### Add support to other microcontrollers

1. Add the a new makefile definitions in "devices" folder, eg, **_myDevice.mk_**
2. Edit the device name, frequency, fuses, lock bits, flash and bootloader parameters.

## Bootloader Logic

### Enter Bootloader Mode
1. The bootloader waits for **_Sync Command_ (SYN [0x16])**. If sync command not received for a while, jump to user program.
2. When receive sync, the bootloader respose with **_Acknowledge Command (ACK [0x06])_** (1 byte), the **_BOOTLOADER VERSION_** (3 bytes), the **_CPU MODEL_** (20 bytes), the **_PAGE_SIZE_** (2 bytes) and the available **_MEMORY_SIZE_** for program (2 bytes), 28 bytes in total (see figure 1).
3. The bootloader wait for the **_Enter Bootloader command (SI [0x0F])_** or the **_Cancel Bootloader command (ESC [0x1B])_**. If some command is not received for a while, jump to user program.
4. If the bootloader receives the **_Cancel Bootloader command_**, jump to user program, else if receives the **_Enter Bootloader command_** enter in bootloader mode
5. The bootloader response with **_Acknowledge Command (ACK [0x06])_** to the **_Enter Bootloader command_** or the **_Cancel Bootloader command_**.
6. Once entered in **_Bootloader mode_**, the bootloader can receive 2 command types: **_Page Write Command (STX [0x02])_** or **_End of Transmission Program (EOT [0x04])_**.

##### Figure 1

	     ACK      VERSION    CPU MODEL   PAGE SIZE   FLASH SIZE
	┌──────────┬───────────┬───────────┬───────────┬────────────┐
	│ 1 (0x06) │     3     │    20     │      2    │      2     | = 28 bytes
	└──────────┴───────────┴───────────┴───────────┴────────────┘

#

### Send a program to bootloader
The loading of a program is done after entering the bootloader mode, sending the **_Page Write Command (STX [0x02])_** followed by the page data chunks of size **_PAGE_SIZE_**. At the end of each packet (STX + DATA_PAGE) the booloader responds with **_Acknowledge Command (ACK [0x06])_**.

The program to be loaded must be fragmented into **_N_** pages of size **_PAGE_SIZE_** data. If the size of the program is not a multiple of **_PAGE_SIZE_**, the remaining data of the program must be added to the last page and filled with **_0xFF_** until the size of **_PAGE_SIZE_** is completed.

The bootloader response with **_No Acknowledge Command (NACK [0x15])_** if **_PAGE_SIZE_** bytes not received in a while after a **_Page Wirte Command (STX [0x02])_**.

Once the loading of the program has been completed, the **_End of Transmission Program (EOT [0x04])_** must be sent to tell the bootloader that the loading has finished. The bootloader responds with **_Acknowledge Command (ACK [0x06])_**  to indicate that it has left the bootloader mode and will execute the program.

If the bootloader receives an invalid command, it will respond with the **_Cancelation Command (CAN [0x18])_**].

1. Send **_Page Write Command (STX [0x02])_**
2. Once the **_Page Write Command_** is received, bootloader responds with **_Acknowledge Command (ACK [0x06])_**
3. Send data in chunks of size **_PAGE_SIZE_**
4. Once the **_PAGE_SIZE_** data is received, bootloader responds with **_Acknowledge Command (ACK [0x06])_**. If the bootloader not receive **_PAGE_SIZE_** chunk in a while, it will respose with **_No Acknowledge Command (NACK [0x15])_** and exit from bootloader mode.
5. Repeat steps 1 to 4 while there is still data to be sent.
6. Send the **_End of Transmission Program (EOT [0x04])_** command to finish loading.
7. The bootloader responds with **_Acknowledge Command (ACK [0x06])_** to indicate that it has left the bootloader mode, and it will jump to the user program.

---