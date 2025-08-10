#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h> // Importa la definicion de NULL
#include "custom_bool.h"

#define MAX_CANT_CLKS 250
#define MAX_CANT_CLKS_250 250
#define MAX_CANT_CLKS_250_250 5

// Hacer los eventos activados globales??
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
	 * En un periodo de X segundos cambiara
	 * la luz del led, de apagado a encendido o
	 * de encendido a apagado
	 */

	// Counter initialization
	cant_clks = 0;
	cant_clks_250 = 0;
	cant_clks_250_250 = 0;

	// Set port output/input
	PORTD &= 0xFB;

	// CLK I/0 prescaler
	TCCR0B |= 0x01; // No prescalling
	TCCR0B &= 0xF1;
	// Timercounter control mask 0
	TIMSK0 |= 0x01; // Starts the timer

	// enable global-interrupt
	sei();

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
