#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h> // Importa la definicion de NULL
#include "custom_bool.h"

#define MAX_CANT_INT 61
#define MAX_CANT_SECS 5

// Hacer los eventos activados globales??
custom_bool time_passed;
uint8_t cant_clks;
uint8_t cant_secs;

void update_cant_clks(void);
void update_cant_secs(void);

// Two posible states
void switch_off( uint8_t *const p_current_state, const uint8_t p_current_action );

void switch_on( uint8_t *const p_current_state, const uint8_t p_current_action );

// Aqui va la subrutina de atencion a la interrupcion
ISR(TIMER0_OVF_vect) // Es una macro definida en avr/interrupt.h
{
	update_cant_clks();

	// No hace falta escribir el reti
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
	cant_secs = 0;
	
	// Set port output/input
	PORTD &= 0xFB;

	// CLK I/0 prescaler
	TCCR0B |= 0x05; // No prescalling
	TCCR0B &= 0xF5;
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
	if ( cant_clks >= MAX_CANT_INT )
	{
		cant_clks = 0;
		update_cant_secs();
	}
}

void update_cant_secs(void)
{
	cant_secs++;
	if ( cant_secs >= MAX_CANT_SECS )
	{
		cant_secs = 0;
		time_passed = true;
	}
}
