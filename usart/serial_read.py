import serial

ser = serial.Serial("COM3", baudrate=9600)

while True:
   data = ser.readline()
   print(data)
