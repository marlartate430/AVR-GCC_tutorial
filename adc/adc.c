#include <avr/io.h>
#include <avr/interrupt.h>
#include "custom_bool.h"

uint8_t saved_value;
custom_bool sendable;
custom_bool new_value;

void power_on_usart(void);

ISR(ADC_vect)
{
	saved_value = ADCH; // If the ADC is set left adjusted, the result will be an 8-bit value stored in ADCH
	new_value = true;
}

ISR(USART_UDRE_vect)
{
	sendable = true;
}

int main(void)
{
	power_on_usart();
	
	sei();
	
	/*
	ADMUX |= 0x60;
	// 0 1 1 0 _ 0 0 0 0

	ADCSRA |= 0xC8;
	// 1 1 0 0 _ 1 0 0 0
	*/
	
	while ( true )
	{
		if ( sendable && new_value)
		{
			UDR0 = saved_value;
			new_value = false;
			sendable = false;
		}

		else if ( sendable )
		{
			UDR0 = 'f';
		}
	}

	return 0;
}

void power_on_usart(void)
{
	UBRR0H |= 0x00;
	UBRR0L |= 0x67;

	UCSR0A |= 0x00;
	UCSR0B |= 0x28;
	// 0 0 1 0 _ 1 0 0 0
	UCSR0C |= 0x06;
	// 0 0 0 0 _ 0 1 1 0
}
