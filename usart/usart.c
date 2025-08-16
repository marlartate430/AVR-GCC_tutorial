#include <avr/io.h>
#include <avr/interrupt.h>
#include "custom_bool.h"
#include <stddef.h>

custom_bool repeat;
custom_bool sendable;
uint8_t current_index;

void power_on_usart(void);
void send_message(void);
void update_counter(uint8_t* const p_current_counter, const uint8_t p_max);

ISR(USART_UDRE_vect)
{
	if ( repeat )
	{
		sendable = true;
	}
}

int main(void)
{
	repeat = false;
	power_on_usart();
	
	current_index = 0;
	
	sei();

	UDR0 = '4';
	repeat = true;

	while ( repeat )
	{
		if ( sendable )
		{
			send_message();
			sendable = false;
		}
	}

	return 0;
}

void power_on_usart(void)
{
	UBRR0H |= 0x00;
	UBRR0L |= 0x67;
	
	UCSR0A |= 0x00;

	UCSR0C |= 0x26;
	// 0 0 1 0 _ 0 1 1 0

	UCSR0B |= 0x28;
	// 0 0 1 0 _ 1 0 0 0
	
}

void send_message(void)
{
	#define ARRAY_SIZE 3
	char possible_letters[ARRAY_SIZE] = { '4', 'K', '\n' };
	char msg = possible_letters[current_index];
	
	update_counter(&current_index, ARRAY_SIZE - 1);

	UDR0 = msg;

}

void update_counter(uint8_t* const p_current_counter, const uint8_t p_max)
{
	if ( p_current_counter != NULL )
	{
		if ( *p_current_counter == p_max )
		{
			*p_current_counter = 0;
		}
		else
		{
			*p_current_counter = *p_current_counter + 1;
		}
	}
}
