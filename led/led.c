 #include <avr/io.h>

int main(void)
{
	// Se va a intentar encender un led
	DDRD |= 0x04; // Salida
	PORTD |= 0x04; // Encender puerto
	return 0;
}
