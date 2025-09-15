#include <avr/io.h>
#include <avr/interrupt.h>
#include "custom_bool.h"
#include <stddef.h>

#define TAMANO_RGB 3
#define OFFSET_PRIMER_PIN 2

/*
 * ENUNCIADO: Se va a enviar desde
 * el PC un numero. Si el numero es
 * del 1 al 3, tiene asociado un led,
 * que se encendera.
 */ 

uint8_t evento;
uint8_t dato_recivido;

ISR(USART_UDRE_vect)
{
	evento = 1;
	dato_recivido = UDR0;
}

void inicializar_usart(void);
void encender_led(const uint8_t p_dato)

int main(void)
{
	// inicializar puertos
	DDRD |= 0x1C;
	// 0 0 0 1 _ 1 1 0 0
	inicializar_usart();
	sei(); // Habilitar Global Interrupt Enable

	while ( true )
	{
		if ( evento == 1 )
		{
			evento = 0;
			encender_led(dato_recivido);
		}
	}

	return 0;
}

void encender_led(const uint8_t p_dato)
{
	const uint8_t dato_restado = p_dato - 1;
	
	if ( dato_restado < TAMANO_RGB )
	{
		for ( uint8_t indice = 0; indice < TAMANO_RGB; indice++ )
		{
			if ( dato_restado == indice )
			{
				PORTD |= ( 1 << ( indice + OFFSET_PRIMER_PIN ) );
			}
			else
			{
				PORTD |= ~( 1 << ( indice + OFFSET_PRIMER_PIN ) );
			}
		}
	}
}

void inicializar_usart(void)
{
	/*
	 * Primero se establece el Baudio
	 * del perfiferico,
	 * despues el formato del frame y
	 * por ultimo se habilita el receptor.
	 */

	/*
	 * OJO: Para este caso va a ser ASINCRONO,
	 * porque no se puede sincronizar el USB
	 * del PC con el de la placa.
	 */

	UBRR0H |= 0x00;
	UBRR0L |= 0x67;
	/*
	 * Calculo del baudio: En el datasheet
	 * indica que el baudio, cuando el modo es
	 * el asincrono, se calcula de la siguiente
	 * forma:
	 * BAUDIO = f_{osc}/(16 * (UBRR_{n} - 1) )
	 * UBRR_{n} es un registro de 16 bits. Entonces,
	 * se despeja UBRR_{n} y el objetivo se convierte 
	 * en calcular dicho registro, donde BAUDIO
	 * es el parametro de la funcion. 
	 */

	UCSR0A |= 0x00;

	UCSR0C |= 0x06;
	// 0 0 0 0 _ 0 1 1 0
	/*
	 * UMSEL01 UMSEL00 : 0 0 => Modo asincrono.
	 * 
	 * UPM01 UPM00 : 0 0 => Sin paridad.
	 * 
	 * USBS : 0 => 1 bit de stop.
	 * 
	 * UCSZ02 UCSZ01 UCSZ00: 0 1 1 => Dato de 8 bits.
	 * 
	 * UCPOL0 : 0 => Flancos de bajada.
	 */

	UCSR0B |= 0x34;
	// 0 0 1 1 _ 0 1 0 0
	/*
	 * RXCIE0 : 0 => Habilitar interrupcion de
	 * recepcion completada.
	 *
	 * TXCIE0 : 0 => No habilitar interrupcion de
	 * recepcion completada.
	 *
	 * UDRIE0 : 1 => No habilitar la interrupcion de
	 * registro de envio vacio.
	 *
	 * RXEN0 : 1 => Habilitar la lectura del USART.
	 *
	 * TXEN0 : 0 => No habilitar la transimision de
	 * archivos.
	 *
	 * UCSZ02 : Bit de tamaino de transmision/lectura
	 * que se menciona en UCSR0C.
	 *
	 * RXB80 : Noveno bit del dato recivido, en caso de
	 * que se vayan a leer mas de 8 bits.
	 *
	 * TXB80 : Noveno bit del dato a enviar, en caso de que
	 * se vayan a transmitir mas de 8 bits.
	 */
}
