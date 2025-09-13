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
Se trataba de aprender a usar los puertos de salida, conectando un led en dicho puerto, tal y como se muestra en la figura 1. El código se encuentra en la carpeta *led*.
!(markdown_resources/led_schematic.jpeg)
