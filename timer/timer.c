#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h> // Importa la definicion de NULL
#include "custom_bool.h"

#define MAX_CANT_CLKS 250 // Cantidad de interrupciones que tienen ocurrir
#define MAX_CANT_CLKS_250 250 // Cantidad de unidades de 250 interrupciones que tienen que ocurrir
#define MAX_CANT_CLKS_250_250 5 // Cantidad de segundos que tienen que ocurrir

/* EXPLICACION
 * El timer0, entre otras funciones como PWM o Clear Timer on Compare Match(CTC),
 * tiene la función "normal" de contar. El contador del timer es el
 * registro TCNT0 de 8 bits, que empieza en 0 y cuando hace un overflow,
 * pasar de 255 a 0 otra vez, salta la interrupcion de desvordamiento
 * del registro, y asi todo el tiempo.
 *
 * Para este modo se tendran en cuenta los siguientes registros:
 * 	* TCNT0: Contador del Timer0, registro de 8 bits.
 * 	
 * 	* TCCR0A: Registro de 8 bits que guarda lo siguiente. Se va a ir
 *	mostrando cada pin del mas significativo al menos significativo:
 * 		- COM0A1
 * 		- COM0A0
 * 		- COM0B1
 * 		- COM0B0
 * 		- Reservado
 * 		- Reservado
 * 		- WGM01
 * 		- WGM00
 *	
 *	COM0A1 COM0A0 forman una combinacion de dos bits, que segun los valores
 * 	que almacenan el timer actua de las siguientes maneras:
 * 		- 0 0: No ocurre nada. El pin 6 actua normal.
 * 		- 0 1: Cada vez que TCNT0 valga el valor guardado en OCR0A, el pin
 *		OC0A invertira su seinal. Siempre que el flag WGM02 del registro
 *		TCCR0B este en alto.
 *		- 1 0: Cada vez que TCNT0 valga el valor guardado en OCR0A, el pin
 *		OC0A pondra en bajo su seinal y cuando TCNT0 haga overflow lo pondra
 *		en alto.
 *		- 1 1: Cada vez que TCNT0 valga el valor guardado en OCR0A, el pin
 *		OC0A pondra en alto su seinal y cuando TCNT0 haga overflow lo pondra
 *		en bajo.
 *	
 *	COM0B1 COM0B0 forman una combinacion de dos bits, que segun los valores
 * 	que almacenan el timer actua de las siguientes maneras:
 * 		- 0 0: No ocurre nada. El pin 5 actua normal.
 *		- 0 1: Reservado
 *		- 1 0: Cada vez que TCNT0 valga el valor guardado en OCR0B, el pin
 *		OC0B pondra en bajo su seinal y cuando TCNT0 haga overflow lo pondra
 *		en alto.
 *		- 1 1: Cada vez que TCNT0 valga el valor guardado en OCR0B, el pin
 *		OC0B pondra en alto su seinal y cuando TCNT0 haga overflow lo pondra
 *		en bajo.
 *
 *	WGM02 del registro TCCR0B y WGM01 WGM00 del registro TCCR0A forman una
 *	combinacion de tres bits cuyo valor indica el modo del timer0. Los dos
 *	valores que indican los modos Fast PWM son los siguientes:
 *		- 0 0 0: El maximo valor que puede alcanzar el contador TCNT0 es
 *		0xFF(255). En este modo tanto el OC0A como OC0B funcionan como
 *		pines del PWM0.
 *		- 1 0 0: El maximo valor que puede alcanzar el contador TCNT0 lo
 *		indica OCR0A. Este modo es usado para el modo CTC
 *
 *	* TCCR0B: Registro de 8 bits que guarda lo siguiente. Se va a ir
 * 	mostrando cada pin del mas significativo al menos significativo:
 *		- FC0A
 *		- FC0B
 *		- Reservado
 *		- Reservado
 *		- WGM02
 *		- CS02
 *		- CS01
 *		- CS00
 *
 *	Los primeros 4 bits no se utilizan para el Timer0.
 *	WGM02 es el bit de la combinacion WGM02 WGM01 WGM00 que se hizo referencia antes.
 *	CS02 CS01 CS00 es una combinacion de bits que sirve para decidir el reloj en el que 
 *	TCNT0 contara. Estas son las opciones:
 *		- 0 0 0: Reloj apagado
 *		- 0 0 1: Reloj sin prescaler
 *		- 0 1 0: Reloj con prescaler de 8
 *		- 0 1 1: Reloj con prescaler de 64
 *		- 1 0 0: Reloj con prescaler de 256
 *		- 1 0 1: Reloj con prescaler de 1024
 *		- 1 1 0: Reloj externo en el pin T0 en flanco de bajada
 *		- 1 1 1: Reloj externo en el pin T0 en flanco de subida
 *
 *	Si no se selecciona el tipo de reloj, no contara el timer. Esto implica a que el
 *	Timer0 no funcionara y su interupcion nunca saltara.
 *
 *	* TIMSK0: Registro de 8 bits que se encarga de habilitar la
 *	interrupcion por overflow. Hay 3 tipos de interrupciones overflow que
 *	se activan en este registro. Los bits que componen el
 *	registro son los siguientes:
 *		- Reservado
 *		- Reservado
 *		- Reservado
 *		- Reservado
 *		- Reservado
 *		- OCIE0B
 *		- OCIE0A
 *		- TOIE0
 *	
 *	OCIE0B: Habilita la interrupcion del Timer0, cuando el TCNT0 alcanza el valor
 *	almacenado en el registro OCR0B.
 *	
 *	OCIE0A: Habilita la interrupcion del Timer0, cuando el TCNT0
 *	alcanza el valor alamcenado en el registro OCR0A.
 *	
 *	TOIE0: Hablita la interrupcion del Timer0, cuando el TCNT0
 *	pasa de 255 a 0, que es lo mismo que un desbordamiento.
 *
 *	OJO!!!: Para que estas interrupciones sean habilitadas, aparte de
 *	activar estos pines hace falta que el Global Interrupt Enable
 *	este habilitado, que se encuentra en el resgitro SREG
 *	( STATUS REGISTER ) en el primer bit.
 */ 

// Hacer los eventos activados globales
custom_bool time_passed;
uint8_t cant_clks;
uint8_t cant_clks_250;
uint8_t cant_clks_250_250;

void update_cant_clks(void);

void update_cant_clks_250(void);

void update_cant_clks_250_250(void);

// Two posible states
void switch_off( uint8_t *const p_current_state, const uint8_t p_current_action );

void switch_on( uint8_t *const p_current_state, const uint8_t p_current_action );


// Aqui va la subrutina de atencion a la interrupcion
ISR(TIMER0_OVF_vect) // Es una macro definida en avr/interrupt.h
{
	update_cant_clks();

	// No hace falta escribir el reti
	// reti()
}

int main(void)
{
	/*
	 * En un periodo de 5 segundos cambiara
	 * la luz del led, de apagado a encendido o
	 * de encendido a apagado
	 */

	// Counter initialization
	cant_clks = 0;
	cant_clks_250 = 0;
	cant_clks_250_250 = 0;

	// Set port output/input
	DDRD |= 0x04;
	PORTD &= 0xFB;
	// 0 0 0 0 _ 1 0 0 0

	// TCCR0A |= 0x00
	/*
	 * COM0A1 COM0A0 : 0 0 => El pin 6 actua de forma normal. No es reemplazado por OC0A.
	 * COM0B1 COM0B0 : 0 0 => El pin 5 actua de forma normal. No es reemplazado por OC0B.
	 * WGM01 WGM00 : 0 0
	 */
	
	TCCR0B |= 0x01; // No prescalling
	TCCR0B &= 0xF1;
	// 0 0 0 0 _ 0 0 0 1
	/*
	 * WGM02 : 0 => WGM02 WMG01 WGM00 : 0 0 0 => El maximo
	 * valor que alcanza el TCNT0 es el maximo, es decir, 255.
	 * Y el modo del timer es el modo de conteo normal.
	 *
	 * CS2 CS1 CS0: 0 0 0 1 => No tiene prescaler,
	 * pero el timer empieza a contar.
	 */

	// Timercounter control mask 0
	TIMSK0 |= 0x01;
	/*
	 * OCIE0B : 0. La interrupcion del Timer0 cuando el TCNT0 cuando
	 * alcanza el valor almacenado en OCR0B esta desactivado.
	 *
	 * OCIE0A : 0. La interrupcion del Timer0 cuando el TCNT0 cuando
	 * alcanza el valor almacenado en OCR0A esta desactivado.
	 *
	 * TOIE0 : 1. La interrupcion del Timer0 cuando realiza un overflow
	 * esta habilitado.
	 *
	 */

	sei(); // Asi se activa el Global Interrupt Enable

	// Generador de eventos
	uint8_t current_state = 0;
	
	while ( true )
	{
		uint8_t current_action = 0;
		
		if ( time_passed )
		{
			current_action = 1;
			time_passed = false;
		}
			
		// Maquina de estados
		if ( current_state == 0 )
		{
			// Maquina de eventos/estado_actual
			switch_on(&current_state, current_action);
		}

		else if ( current_state == 1 )
		{
			// Maquina de eventos/estado_actual
			switch_off(&current_state, current_action);
		}

	}

	return 0;
}

void switch_off( uint8_t *const p_current_state, const uint8_t p_current_action )
{
	if ( p_current_action == 1 && p_current_state != NULL )
	{
		*p_current_state = 0;
		PORTD &= 0xFB; // Apagar pin
	}
}

void switch_on( uint8_t *const p_current_state, const uint8_t p_current_action )
{
	if ( p_current_action == 1 && p_current_state != NULL )
	{
		*p_current_state = 1;
		PORTD |= 0x04; // Encender pin
	}
}

void update_cant_clks(void)
{
	cant_clks++;
	if ( cant_clks >= MAX_CANT_CLKS )
	{
		cant_clks = 0;
		update_cant_clks_250();
	}
}

void update_cant_clks_250(void)
{
	cant_clks_250++;
	if ( cant_clks_250 >= MAX_CANT_CLKS_250 )
	{
		cant_clks_250 = 0;
		update_cant_clks_250_250();
	}
}

void update_cant_clks_250_250(void)
{
	cant_clks_250_250++;
	if ( cant_clks_250_250 >= MAX_CANT_CLKS_250_250 )
	{
		cant_clks_250_250 = 0;
		// Aqui activas el evento que se ha activado
		time_passed = true;
	}
}
