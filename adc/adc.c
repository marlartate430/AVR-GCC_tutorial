#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t saved_value;

ISR(ADC_vect)
{
	saved_value = ADCH; // If the ADC is set left adjusted, the result will be an 8-bit value stored in ADCH
}

int main(void)
{
	sei();
	
	ADMUX |= 0x60;
	// 0 1 1 0 _ 0 0 0 0

	ADCSRA |= 0xC8;
	// 1 1 0 0 _ 1 0 0 0

	return 0;
}
