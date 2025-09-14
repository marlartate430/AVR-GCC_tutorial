#include <avr/io.h>
#include <stdint.h>

/*
 * EXPLICACION
 * Los pines que controlan los PWMs son los mismos que los pines
 * manejados antes. La cosa es que estan indicados en la placa
 * Arduino por un "~". Para poder activarlos, hay que marcalos como
 * bits de salida en el registro DDRD, como con pines normales de salida.
 *
 * En este micro, los Timer ademas de valer como contadores tambien tienen
 * la funcion de PWM mediante los modos Fast PWM y Correct Phase PWM. En
 * este caso se explicara el primero.
 *
 * Para este modo se van a tener en cuenta los siguientes registros:
 * 	* TCNT0: Contador del Timer0, registro de 8 bits.
 * 	
 * 	* OCR0A: Registro de 8 bits que, dependiendo del modo tiene una funcion diferente.
 * 	Ademas, tiene asociado un pin 0C0A.
 * 	
 * 	* 0C0A: Pin que corresponde al pin 6 de la placa. Si esta activado el
 * 	el modo Fast PWM, reemplaza el funcionamiento normal del pin 6.
 * 	
 * 	* 0C0B: Pin que corresponde al pin 5 de la placa. Si esta activado el
 * 	el modo Fast PWM, reemplaza el funcionamiento normal del pin 5.
 *
 * 	* OCR0B: Registro de 8 bits, cuyo valor almacenado se compara constantemente
 * 	con el de TCNT0. En caso de que sean iguales el pin 0C0B,
 * 	que tiene asociado dicho regsitro, cambiara su tension
 * 	en funcion del comportamiendo que tenga almacenado el registro TCCR0A
 * 	
 * 	* TCCR0A: Registro de 8 bits que guarda lo siguiente. Se va a ir
 * 	mostrando cada pin del mas significativo al menos significativo:
 * 		- COM0A1
 * 		- COM0A0
 * 		- COM0B1
 * 		- COM0B0
 * 		- Reservado
 * 		- Reservado
 * 		- WGM01
 * 		- WGM00
 *
 * 	COM0A1 COM0A0 forman una combinacion de dos bits, que segun los valores
 * 	que almacenan el PWM actua de las siguientes maneras:
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
 * 	que almacenan el PWM actua de las siguientes maneras:
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
 *		- 0 1 1: El maximo valor que puede alcanzar el contador TCNT0 es
 *		0xFF(255). En este modo tanto el OC0A como OC0B funcionan como
 *		pines del PWM0.
 *		- 1 1 1: El maximo valor que puede alcanzar el contador TCNT0 lo
 *		indica OCR0A. Por tanto, el pin asociado OCR0A(OC0A) no puede ser
 *		usado como pin de PWM, pero el OC0B funciona igual.
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
 *	Los primeros 4 bits no se utilizan para el PWM.
 *	WGM02 es el bit de la combinacion WGM02 WGM01 WGM00 que se hace referencia antes.
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
 *	PWM no funcionara.
 */ 

int main(void)
{
	// Set output PWM pins high ( COMPULSORY )
	DDRD |= 0x60;
	DDRD &= 0x6F;
	// 0 1 1 0 _ 0 0 0 0 
	// Pines 5(OC0B) y 6(OC0A)

	// Set Timer 0 to Fast PWM
	TCCR0A |= 0xF3;
	// 1 1 1 1 _ 0 0 1 1
	
	/* COM0A1 COM0A0 : 1 1 => Cada vez que TCNT0 valga
	 * el valor guardado en OCR0A, el pin OC0A pondra
	 * en alto su seinal y cuando TCNT0 haga overflow
	 * lo pondra en bajo.
	 *
	 * COM0B1 COM0B0 : 1 1 => Lo mismo que con COM0A1 y
	 * COM0A0, pero con el pin OC0B.
	 *
	 * WGM01 y WGM00 : 1 1 => Se dira mas tarde cuando se
	 * conozca el valor de WGM02.
	 */

	TCCR0B |= 0x01;
	// 0 0 0 0 _ 0 0 0 1
	
	/*
	 * WGM02 : 0 => WGM02 WMG01 WGM00 : 0 1 1 => El maximo
	 * valor que alcanza el TCNT0 es el maximo, es decir, 255.
	 *
	 * CS02 CS01 CS00 : 0 0 1 => Reloj en funcionamiento
	 * pero sin prescaler.
	 */ 

	OCR0A = 0x00;
	/* En cuanto el contador TCNT0 valga 0,
	 * la seinal OC0A se encendera. Cuando
	 * el contador llegue al valor maximo,
	 * en este caso 255, se reiniciara y se
	 * apagara.
	 */
	OCR0B = 0x7F;
	/* En cuanto el contador valga 128,
	 * la seinal OC0B se encendera. Cuando
	 * el contador llegue al valor maximo,
	 * en este caso 255, se reiniciara y se
	 * apagara.
	 */

	// TCCR0A &= 0xFF // We don't know if the reserved bits are set or not and we have to keep the bits of the first digit, so it would result an and of FF, which is the same as not doing anything
	
	return 0;
}
