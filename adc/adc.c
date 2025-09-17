#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "custom_bool.h"

#define ASCII_OFFSET '0'

uint8_t evento;
uint16_t numero_invertido;
uint8_t estado_actual;

uint8_t valor_adc;

custom_bool int_usart;
custom_bool int_adc;

/*
 * EVENTOS:
 * Bit 0 : adc terminado
 * Bit 1 : usart vacio
 * Bit 2 : numero vacio
 */

ISR(USART_UDRE_vect) 
{
	int_usart = true;
}

ISR(ADC_vect)
{
	int_adc = true;
	valor_adc = ADCH;
}

void inicializar_adc(void);
void inicializar_usart(void);

void apagar_adc(void);
void apagar_usart(void);
void encender_adc(void);
void encender_usart(void);

void comenzar_conversion(void);

void generador_eventos(void);

void estado1(void); // Estado del ADC
void estado2(void); // Estado de enviar la cadena
void estado3(void); // Estado de transicion al primero

void enviar_caracter( const int8_t p_caracter );
void enviar_numero_invertido(void);
uint16_t invertir_numero(const uint8_t p_num);

int main(void)
{

	DDRD |= 0x1C;
	// 0 0 0 1 _ 1 1 0 0

	evento = 0;
	estado_actual = 1;

	inicializar_usart();
	inicializar_adc();

	sei();

	encender_adc();

	comenzar_conversion();

	while ( true )
	{
		generador_eventos();

		if ( estado_actual == 1 )
		{
			estado1();
		}
		else if ( estado_actual == 2 )
		{
			estado2();
		}
		else if ( estado_actual == 3 )
		{
			estado3();
		}
	}

	return 0;
}


void inicializar_usart(void)
{
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
}

void inicializar_adc(void)
{
	/* Voltage Reference AVcc, en la placa Arduino UNO R3
	 * es de 5 V.
	 */
	// Left adjust result
	// analog channel 0
	
	ADMUX |= 0x60;
	// 0 1 1 0 _ 0 0 0 0
	
	// ADC enable
	// ADC no auto trigger
	// ADC Interrupt enable
	// ADC prescaler division factor = 2
	
	ADCSRA |= 0x8F;
	// 1 0 0 0 _ 1 1 1 1
}

void encender_usart(void)
{
	UCSR0B |= 0x28;
	// 0 0 1 0 _ 1 0 0 0
	/*
	 * RXCIE0 : 0 => Habilitar interrupcion de
	 * recepcion completada.
	 *
	 * TXCIE0 : 0 => No habilitar interrupcion de
	 * recepcion completada.
	 *
	 * UDRIE0 : 1 => Habilitar la interrupcion de
	 * registro de envio vacio.
	 *
	 * RXEN0 : 0 => No habilitar la lectura del USART.
	 *
	 * TXEN0 : 1 => Habilitar la transimision de
	 * archivos.
	 *
	 * UCSZ02 : 0 => Bit de tamaino de transmision/lectura
	 * que se menciona en UCSR0C.
	 *
	 * RXB80 : 0 => Noveno bit del dato recivido, en caso de
	 * que se vayan a leer mas de 8 bits.
	 *
	 * TXB80 : 0 => Noveno bit del dato a enviar, en caso de que
	 * se vayan a transmitir mas de 8 bits.
	 */

}

void apagar_usart(void)
{
	UCSR0B &= ~(0x28);
	// ~(0 0 1 0 _ 1 0 0 0)
}

void encender_adc(void)
{
	ADCSRA |= 0x80;
	// 1 0 0 0 _ 0 0 0 0

}

void comenzar_conversion(void)
{
	/* OJO: Tiene que haber un margen de tiempo desde que el adc
	 * se enciende hasta que comienza la conversion. Cuando termina
	 * la conversion, este bit ya se desactiva solo.
	 */

	ADCSRA |= 0x40;
	// 0 1 0 0 _ 0 0 0 0
}

void apagar_adc(void)
{
	ADCSRA &= 0x7F;
	// 0 1 1 1 _ 1 1 1 1
}

void generador_eventos(void)
{
	uint8_t evento_anterior = evento & 0xFD;
	// 1 1 1 1 _ 1 1 0 1
	/* No se van a tener en cuenta si se repiten las
	 * interrupciones.
	 */

	if ( numero_invertido == 0 )
	{
		evento |= 0x04;
		// 0 0 0 0 _ 0 1 0 0
	}

	evento = evento & ( evento_anterior ^ evento );

	if ( int_usart )
	{
		int_usart = false;
		evento |= 0x02;
		// 0 0 0 0 _ 0 0 1 0
	}
	
	if ( int_adc )
	{
		int_adc = false;
		evento |= 0x01;
		// 0 0 0 0 _ 0 0 0 1
	}
}

void estado1(void)
{
	uint8_t evento_de_interes = evento & 0x01; 
	// 0 0 0 0 _ 0 0 0 1
	/* No es importante en este estado saber si el numero_invertido vale 0 o no.
	 * Tampoco interesa saber si el USART esta libre.
	 */

	if ( evento_de_interes == 1 )
	{
		numero_invertido = invertir_numero(valor_adc);
		estado_actual = 2;
		apagar_adc();
		encender_usart();
	}
}

void estado2(void)
{
	uint8_t evento_de_interes = evento & 0x06;
	// 0 0 0 0 _ 0 1 1 0
	/* No es importante saber si el ADC ha terminado.
	 */

	if ( evento_de_interes == 2 )
	{
		enviar_numero_invertido();
	}
	
	else if ( evento_de_interes == 6 )
	{
		UDR0 = '\n';
		estado_actual = 3;
	}
}

void estado3(void)
{
	uint8_t evento_de_interes = evento & 0x02;
	// 0 0 0 0 _ 0 0 1 0
	/* No hace falta saber si el ADC ha terminado.
	 * Tampoco si "numero_invertido" esta vacio.
	 */

	if ( evento_de_interes == 2 )
	{
		apagar_usart();
		encender_adc();
		estado_actual = 1;
		comenzar_conversion();
	}
}

void enviar_caracter( const int8_t p_caracter )
{
	/* pre: Se ha detectado antes de que el USART esta
	 * esta preparado para enviar un caracter.
	 */

	UDR0 = p_caracter;
}

void enviar_numero_invertido(void)
{
	/* pre: Se ha comprobado antes que "numero_invertido"
	 * no vale 0.
	 */

	int8_t digito_actual = numero_invertido % 10 + ASCII_OFFSET;
	enviar_caracter(digito_actual);
	numero_invertido = numero_invertido / 10;

}

uint16_t invertir_numero(const uint8_t p_num)
{
	uint16_t resultado = 0;
	uint8_t num_restante = p_num;

	while ( num_restante != 0 )
	{
		uint8_t base_actual = num_restante % 10;
		resultado = resultado * 10 + base_actual;
		num_restante = num_restante / 10;
	}

	return resultado;
}

