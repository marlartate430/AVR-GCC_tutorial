#include <avr/io.h>
#include <stdint.h>

int main(void)
{
	// Set output PWM pin high ( COMPULSORY )
	DDRB |= 0x40;
	DDRB &= 0x4F;

	// Set Timer 0 to Fast PWM
	TCCR0A |= 0x43;
	// TCCR0A &= 0xFF // We don't know if the reserved bits are set or not and we have to keep the bits of the first digit, so it would result an and of FF, which is the same as not doing anything
	
	return 0;
}
