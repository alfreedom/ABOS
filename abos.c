/******************************************************
 * abos.c
 *
 * Program Name:  abos-bootloader
 *         Date:  2018-08-17
 *       Author:  Alfredo Orozco
 *      License:  GNU
 *      Version:  1.0.0
 *
 * Description:
 *  Bootloader simple para microcontroladores Atmega16/32
 * 
 *****************************************************/
#define BAUD 38400

#include <avr/io.h>
#include <util/delay.h> 
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/setbaud.h>

#define ABOS_VERSION_MA 1
#define ABOS_VERSION_MI 0
#define ABOS_VERSION_IS 0

#ifdef UDR

#define UC_UDR	UDR
#define UC_UCSRC	UCSRC
#define UC_UCSRB	UCSRB
#define UC_UCSRA	UCSRA
#define UC_UDRE UDRE
#define UC_RXC RXC
#define UC_UBRRL UBRRL
#define UC_UBRRH UBRRH
#define UC_URSEL URSEL
#define UC_UCSZ0 UCSZ0
#define UC_UCSZ1 UCSZ1
#define UC_RXEN RXEN
#define UC_TXEN TXEN

#elif defined(UDR0)			

#define UC_UDR UDR0
#define UC_UCSRC	UCSR0C
#define UC_UCSRB	UCSR0B
#define UC_UCSRA	UCSR0A
#define UC_UDRE UDRE0
#define UC_RXC RXC0
#define UC_UBRRL UBRR0L
#define UC_UBRRH UBRR0H
#define UC_URSEL 0
#define UC_UCSZ0 UCSZ00
#define UC_UCSZ1 UCSZ01
#define UC_RXEN RXEN0
#define UC_TXEN TXEN0

#elif defined(UDR1)			

#define UC_UDR    UDR1
#define UC_UCSRC	UCSR1C
#define UC_UCSRB	UCSR1B
#define UC_UCSRA	UCSR1A
#define UC_UDRE   UDRE1
#define UC_RXC    RXC1
#define UC_UBRRL  UBRR1L
#define UC_UBRRH  UBRR1H
#define UC_URSEL  0
#define UC_UCSZ0  UCSZ10
#define UC_UCSZ1  UCSZ11
#define UC_RXEN   RXEN1
#define UC_TXEN   TXEN1

#endif

#define SYNC_COMMAND							0x16
#define ENTER_BOOTLOADER_COMMAND	0x0F
#define END_TRANSMISSION_COMMAND	0x04
#define ACK_COMMAND								0x06
#define NACK_COMMAND							0x15
#define CANCEL_BOOTLOADER_COMMAND	0x1B
#define PAGE_WRITE_COMMAND				0x02
#define CANCELATION_COMMAND				0x18

#define MAX_RECEIVE_TRIES 700

void UsartWrite(uint8_t byte);
int UsartRead();
void Bootloader();

int main(void) {

	int recv;
	/* Inicializa la USART */
	UC_UBRRL = UBRRL_VALUE;
	UC_UBRRH = UBRRH_VALUE;
	UC_UCSRC = (1 << UC_URSEL) | (1 << UC_UCSZ0) | (1 << UC_UCSZ1);
	UC_UCSRB = (1 << UC_RXEN) | (1 << UC_TXEN);
	
	/* Si se recibe el caracter SYN entra en modo bootloader */
	if(UsartRead() == SYNC_COMMAND)
	{
		/* Envía ACK */
		UsartWrite(ACK_COMMAND);

		/* Envia el tamaño de página */
		UsartWrite((uint8_t)SPM_PAGESIZE);
		UsartWrite(SPM_PAGESIZE >> 8);

		/* Envia el espacio disponible para el programa */
		UsartWrite((uint8_t)((FLASH_SIZE-BOOT_SIZE)/1024));
		UsartWrite(((FLASH_SIZE - BOOT_SIZE)/1024) >> 8);

		/* Envia la versión del ABOS */
		UsartWrite((uint8_t)ABOS_VERSION_MA);
		UsartWrite((uint8_t)ABOS_VERSION_MI);
		UsartWrite((uint8_t)ABOS_VERSION_IS);

		/* Espera comando SYNC para entrar en modo bootloader o 
		   ESC para cancelar la carga */
		while((recv = UsartRead()) != ENTER_BOOTLOADER_COMMAND && recv != CANCEL_BOOTLOADER_COMMAND);
		if(recv == ENTER_BOOTLOADER_COMMAND || recv == CANCEL_BOOTLOADER_COMMAND)
			UC_UDR = ACK_COMMAND;

		/* Entra en modo bootloader */
		if(recv == ENTER_BOOTLOADER_COMMAND)
			Bootloader();	
		/* Espera 5 ms despues de salir del bootloader */
	}

	/* Salta a la aplicación en la dirección 0 */
	asm("JMP 0");
}

void Bootloader()
{
	int cmd;
	uint8_t exit=0;
	uint16_t i;
  uint8_t sreg;
	uint32_t addr=0;
	uint32_t page = 0;

	while(!exit)
	{
		cmd = UsartRead();
		switch(cmd)
		{
			/* Comando STX, inicia la recepción de una página de datos */
			case PAGE_WRITE_COMMAND:
				/* Calcula la dirección dependiendo de la pagina */
				addr = (page++) * SPM_PAGESIZE;
				/* Guarda el registro SREG */
				sreg = SREG;
				/* Deshabilita interrupciones */
				cli();
				/* Espera a que la memoria EEPROM no este ocupada */
				eeprom_busy_wait();
				/* Borra la página de memoria */
				boot_page_erase(addr);
				/* Espera mientras la página de memoria es borrada */
				boot_spm_busy_wait();
				/* Envía ACK para avisar que está listo para recibir datos */
				UsartWrite(ACK_COMMAND);

				/* Recibe la página a programa desde la USART */
				for (i=0; i<SPM_PAGESIZE; i+=2)
				{
					/* Guarda los datos en formato Little Endian */
					uint16_t w1 = UsartRead();
					uint16_t w2 = UsartRead();
					/* Si no se recibieron datos (timeout) envia NACK y sale del modo bootloader */
					if(w1 == -1 || w2 == -1)
					{
						/* Envía NACK */
						UsartWrite(NACK_COMMAND);
						/* Sale del modo bootloader */
						exit = 1; 
						break;
					}
					/* Une los 2 BYTES en un WORD */
					w1 += (w2) << 8;
					/* Llena la página con el dato y la dirección */
					boot_page_fill(addr + i, w1);
				}

				/* Si hubo un error, sale del modo booloader */
				if(exit == 1)
					break;

				/* Escribe la página en la memoria FLash */
				boot_page_write (addr);     
				/* Espera mientras la memoria Flash es escrita */
				boot_spm_busy_wait();       
				/* Habilita la sección RWW. Es necesario si se quiere saltar
				 a la aplicación despues de salir del bootloader */
				boot_rww_enable ();
				// Habilita interrupciones
				sei();
				/* Restaura el registro SREG */
				SREG = sreg;
				/* Envía ACK para indicar que la página fue escrita y 
				puede recibir mas datos */
				UsartWrite(ACK_COMMAND);
			break;

			/* Comando EOT. Finaliza la carga de la aplicación */
			case END_TRANSMISSION_COMMAND:
				/* Envía ACK para indicar que se ha finalizado correctamente */
				UsartWrite(ACK_COMMAND);
				/* Sale del modo bootloader */
				exit = 1;
			break;
			
			/* Si no se recibio un comando válido sale del modo bootloader */
			default: 
				/* Envía CAN, esperando que el cargador lo reciba. */
				UsartWrite(CANCELATION_COMMAND);
				exit=1; 
			break;
		}
	}
}

/* 
	UsartWrite
		Envía un dato por la USART. 

	Recibe:
		Dato de 8 bits a enviar.
*/
void UsartWrite(uint8_t byte){
	/* Si el transmisor está vacío envía el dato */
	while (!(UC_UCSRA & _BV(UC_UDRE)));
		UC_UDR = byte;
}

/* 
	UsartRead
		Lee un dato recibido en la USART.

	Devuelve:
		El dato recibido por la USAR.
		Si no se recibe un dato en un tiempo TIMEOUT devuelve -1.
*/
int UsartRead(){
	uint32_t timeout = 0;
	/* Mientras no se reciba un dato o mientras no se 
	exeda el tiempo de espera, sigue esperando un dato. */
	while (!(UC_UCSRA & _BV(UC_RXC)) && timeout < MAX_RECEIVE_TRIES)
	{
		_delay_us(500);
		timeout++;
	}

	/* Si salio del ciclo por haber excedio el tiempo 
	de espera devuelve -1, si no devuelve el dato recibido. */
	return timeout == MAX_RECEIVE_TRIES ? -1 : UC_UDR;
}
