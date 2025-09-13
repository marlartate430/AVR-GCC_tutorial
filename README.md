# Tutorial de AVR-GCC en el Atmega328p
El motivo del repositorio era practicar la programación en C en microcontroladores. Por disponibilidad, se ha utilizado una placa Arduino UNO R3, que contiene un microcontrolador Atmega328p. En las siguientes cabeceras se mostrarán los periféricos programados con sus respectivos circuitos, además de unos incisos y consejos al principio.

## Inciso
Cada carpeta contiene un código `.c`, con su respectivo script para compilarlo y pasarlo a la placa tanto para GNU/Linux como para Windows. Los siguientes pasos a realizar son los mismos en los sistemas operativos mencionados previamente:<br>
**Requisitos**<br>
> Es necesario tener accesible desde la terminal el compilador **AVR-GCC** y el programa encargado de la transmisión **AVRDUDE**.<br>

**Compilación y transmisión**<br>
> Entra al directorio correspondiente y en la terminal escriba `make`.

### Alternativa
Una alternativa a la anterior forma de compilación y transferencia del ejecutable a la placa es utilizando el **IDE de Arduino**.
> Habría qe pasar el código `.c` del repositorio a un archivo `.ino` compatible con el IDE y por último compilarlo y ejecutarlo.

## Práctica
### Puertos de salida
Se trata de aprender a usar los puertos de salida, conectando un led en dicho puerto, tal y como se muestra en la figura 1. El código se encuentra en la carpeta *led*.
![](markdown_resources/led_schematic.jpeg)
|:--:|
|Figura1|

> Este circuito **tambien se va a utilizar** en los directorios **Timer**.

### Timer
Este trata de aprender a manejar el Timer 0 del microcontrolador. Para ello, la luz del led cambia su nivel (encendido o apagado) cada 5 segundos.

### PWM
Se vuelve a utilizar el timer0, pero esta vez en su modo PWM. As�, el valor que se le asigne en el registro OCR0A se refleja en la intensidad de la luz del led conectado al puerto 6 y el puerto 5 en función del registro 0CR0B. A continuaci�n se mostrará su circuito en la figura 2.

![](markdown_resources/pwm_schematics.jpg)
|:--:|
|Figura 2|
