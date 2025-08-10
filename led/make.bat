@echo off

set PROGRAMAS=%USERPROFILE%\Desktop\Programas
set AVR=%PROGRAMAS%\avr8-gnu-toolchain-win32_x86_64\bin
set AVRDUDE=%PROGRAMAS%\avrdude
set MICRO_TARGET=atmega328p

if "%1"=="" (
	call :object
	call :elf
	call :hex
	call :load
	goto :eof
)

if "%1"=="object" (
	call :object
	goto :eof
)

if "%1"=="elf" (
	call :elf
	goto :eof
)

if "%1"=="clean" (
	call :clean
	goto :eof
)

if "%1"=="hex" (
	call :hex
	goto :eof
)

if "%1"=="load" (
    call :load
    goto :eof
)

:object
SETLOCAL
set CC=%AVR%\avr-gcc
set OUTPUT=led.o
set FLAGS=-mmcu=%MICRO_TARGET% -o %OUTPUT% -c
set SRC=led.c
%CC% %FLAGS% %SRC%
echo %CC% %FLAGS% %SRC%
ENDLOCAL
goto :eof

:elf
SETLOCAL
set CC=%AVR%\avr-gcc
set OUTPUT=led.elf
set SRC=led.o
set FLAGS=-mmcu=%MICRO_TARGET% -o %OUTPUT%
%CC% %FLAGS% %SRC%
echo %CC% %FLAGS% %SRC%
ENDLOCAL
goto :eof

:clean
del led.o
del led.elf
del led.hex
goto :eof

:hex
SETLOCAL
set CC=%AVR%\avr-objcopy
set OUTPUT=led.hex
set SRC=led.elf
set FLAGS=-j .text -j .data -O ihex %SRC% %OUTPUT%
%CC% %FLAGS%
echo %CC% %FLAGS%
ENDLOCAL
goto :eof

:load
SETLOCAL
set CC=%AVRDUDE%\avrdude
set CONF=%AVRDUDE%\avrdude.conf
set PART=m328p
set PORT=COM3
set SRC=led.hex
set FLAGS=-c arduino -C %CONF% -p %PART% -P %PORT% -U flash:w:%SRC%:i
%CC% %FLAGS%
echo %CC% %FLAGS%
ENDLOCAL
goto :eof
