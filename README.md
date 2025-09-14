# Tutorial de AVR-GCC en el Atmega328p
El motivo del repositorio era practicar la programación en C en microcontroladores. Por disponibilidad, se ha utilizado una placa Arduino UNO R3, que contiene un microcontrolador Atmega328p. En las siguientes cabeceras se mostrarán los periféricos programados con sus respectivos circuitos, además de unos incisos y consejos al principio.

## Documentación ##
Se han usado los documentos oficiales de la placa Arduino UNO R3 y el datasheet oficial del Atega328p. También el uso de Chat GPT ha sido de ayuda, **no para generar código** sino para alguna explicación sobre el funcionamiento del dispositivo que ha sido difícil de entender con la documentación ya mencionada. Además, sitios web como StackOverflow han sido de ayuda. Aquí están los enlaces a los datasheets:
Arduino UNO R3:
![](https://docs.arduino.cc/hardware/uno-rev3/?_gl=1*ngoux9*_up*MQ..*_ga*MjEzMzE0NDM4Ny4xNzU3ODQyMTA2*_ga_NEXN8H46L5*czE3NTc4NDIxMDMkbzEkZzEkdDE3NTc4NDIxMTMkajUwJGwwJGg4NjQwMDUyMzM.)<br>

Atmega328p:
![](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

## Inciso ##
Cada carpeta contiene un código `.c`, con su respectivo script para compilarlo y pasarlo a la placa tanto para GNU/Linux como para Windows. Los siguientes pasos a realizar son los mismos en los sistemas operativos mencionados previamente:<br>
**Requisitos**<br>
> Es necesario tener accesible desde la terminal el compilador **AVR-GCC** y el programa encargado de la transmisión **AVRDUDE**.<br>

**Compilación y transmisión**<br>
> Entra al directorio correspondiente y en la terminal escriba:
```
make
```

### Alternativa ###
Otra forma de compilación y transferencia del ejecutable a la placa es utilizando el **IDE de Arduino**.
> Habrá que pasar el código `.c` del repositorio a un archivo `.ino` compatible con el IDE y por último compilarlo y ejecutarlo.

## Prácticas ##
### Puertos de salida ###
Se trata de aprender a usar los puertos de salida, conectando un led en dicho puerto, tal y como se muestra en la *figura 1*. El código se encuentra en la carpeta `led`.
![](markdown_resources/led_schematic.jpg)
|:--:|
|Figura1|

> Este circuito **también se va a utilizar** en los directorios **Timer**.

### Timer ###
Este trata de aprender a manejar el Timer 0 del microcontrolador. Para ello, la luz del led cambia su nivel (encendido o apagado) cada 5 segundos.

### PWM ###
Se vuelve a utilizar el timer0, pero esta vez en su modo PWM. Así, el valor que se le asigne en el registro OCR0A se refleja en la intensidad de la luz del led conectado al puerto 6 y el puerto 5 (pines 0C0A y 0C0B respectivamente) en función del registro 0CR0B. A continuación se mostrará su circuito en la *figura 2*.

![](markdown_resources/pwm_schematic.jpg)
|:--:|
|Figura 2|

### USART ###
Para este caso se ha utilizado ningún puerto. Solo se envía una cadena de carácteres siempre que el USART esté disponible para enviar un carácter. Después, se puede comprobar el mensaje recibido mediante el lector de puerto serie de Arduino. En GNU/Linux es necesario instalar el paquete *Busca el nombre del paquete*.

### ADC ###
La forma que se obtiene un valor analógico es mediante un divisor de voltaje, tal y como se muestra en la *figura 3*.El programa trata de una maquina de dos estados. En el primero, se espera a que el ADC haya terminado; en ese caso se cambia de al siguiente estado. En el segundo, se envía el dato recibido por el conversor mediante el USART al ordenador.

![](markdown_resources/adc_schematic.jpg)
|:--:|
|Figura 3|
