#!/usr/bin/python
# Python example with serial
# stuff.

import serial

ser = serial.Serial("/dev/ttyUSB0")  # open first serial port
ser.baudrate = 57600
print ser.portstr       # check which port was really used
#ser.write("h0ello")     # write a string
i = 0
while 1:
  output = ser.read(1)     # read a byte
  print "i = %d: %x %c" % (i, ord(output), output )
  i = i + 1
ser.close()             # close port
