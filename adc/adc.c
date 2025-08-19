#include <avr/io.h>
#include <avr/interrupt.h>
#include "custom_bool.h"

#define ASCII_OFFSET 48

uint8_t previous_events;
uint8_t events;
uint8_t states;
uint8_t saved_value;
uint8_t current_cant_digits;

custom_bool last_char;

void initialize_adc(void);
void initialize_usart(void);

void event_generator(void);

void state1(void);
void state2(void);

void switch_off_adc(void);
void switch_on_usart(void);

void switch_off_usart(void);
void switch_on_adc(void);
void start_conversion(void);

uint8_t get_cant_digits(const uint8_t p_number);
uint8_t get_digit(const uint8_t p_number, const uint8_t p_index);

ISR(ADC_vect)
{
	events = 1;
	saved_value = ADCH;	
}

ISR(USART_UDRE_vect)
{
	events = 2;
}

int main(void)
{
	states = 1;
	events = 0;

	DDRD |= 0x40;

	initialize_usart();

	sei();
	
	initialize_adc();
	start_conversion();

	// by default usart is switched off
	switch_on_adc();

	while ( true )
	{
		event_generator();

		if ( states == 1 )
		{
			state1();
		}
		else if ( states == 2 )
		{
			state2();
		}

		previous_events = events;
	}

	return 0;
}

void initialize_adc(void)
{
	// Voltage Reference AVcc
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

void initialize_usart(void)
{
	// Select BAUD rate
	UBRR0H |= 0x00;
	UBRR0L |= 0x67;
	
	// Not enable USART Data register empty interrupt
	// Not enable TXE0
	UCSR0B |= 0x00;
	// 0 0 0 0 _ 0 0 0 0

	// Asynchronous
	// No parity
	// 1 bit stop
	// 8 bit
	UCSR0C |= 0x06;
	// 0 0 0 0 _ 0 1 1 0
}

void event_generator(void)
{
	events = events & ( previous_events ^ events );
}

void state1(void)
{
	if ( events == 1 )
	{
		switch_off_adc();
		switch_on_usart();
		states = 2;
		events = 0;

		last_char = false;

		current_cant_digits = get_cant_digits(saved_value);
	}
}

void state2(void)
{
	if ( events == 2 )
	{
		if ( !last_char )
		{
			if ( current_cant_digits == 0 )
			{
				last_char = true;
				UDR0 = '\n';
			}
			else
			{
				uint8_t current_digit = get_digit(saved_value, current_cant_digits);
				UDR0 = current_digit + ASCII_OFFSET;
				current_cant_digits--;
			}
		}
		else
		{
			switch_off_usart();
			switch_on_adc();
			states = 1;
			start_conversion();
		}

		events = 0;
	}
}

void switch_off_adc(void)
{
	ADCSRA &= 0x7F;
	// 0 1 1 1 _ 1 1 1 1
}

void switch_on_usart(void)
{
	UCSR0B |= 0x28;
	// 0 0 1 0 _ 1 0 0 0 
}

void switch_off_usart(void)
{
	UCSR0B &= 0xD7;
	// 1 1 0 1 _ 0 1 1 1
}

void switch_on_adc(void)
{
	ADCSRA |= 0x80;
	// 1 0 0 0 _ 0 0 0 0
}

void start_conversion(void)
{
	ADCSRA |= 0x40;
	// 0 1 0 0 _ 0 0 0 0
}

uint8_t get_cant_digits(const uint8_t p_number)
{
	uint8_t result = 0;
	uint8_t current_number = p_number;

	while ( current_number != 0 )
	{
		current_number = current_number / 10;
		result++;
	}

	return result;
}

uint8_t get_digit(const uint8_t p_number, const uint8_t p_index)
{
	uint8_t result = p_number;
	uint8_t current_index = p_index;

	while ( current_index != 0 )
	{
		result = result / 10;
		current_index--;
	}

	return result;
}
