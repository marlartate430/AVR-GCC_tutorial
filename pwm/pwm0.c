#include <avr/io.h>
#include <stdint.h>

int main(void)
{
	// Set output PWM pin high ( COMPULSORY )
	DDRD |= 0x40;
	DDRD &= 0x4F;

	// Set Timer 0 to Fast PWM
	TCCR0A |= 0x43;
	// 0 1 0 0 _ 0 0 1 1

	TCCR0B |= 0x0D;
	// 0 0 0 0 _ 1 1 0 1

	OCR0A = 0x7F;

	// TCCR0A &= 0xFF // We don't know if the reserved bits are set or not and we have to keep the bits of the first digit, so it would result an and of FF, which is the same as not doing anything
	
	return 0;
}
