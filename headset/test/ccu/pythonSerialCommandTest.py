#!/usr/bin/python
# Python example with serial
# stuff.

import serial

sl = "ATSL"
sh = "ATSH"
comm = "+++"
ser = serial.Serial("/dev/ttyUSB0")  # open first serial port
ser.baudrate = 57600
print ser.portstr       # check which port was really used
ser.write(comm)     # write a string
out = ser.read(2)
print out
ser.write(sl)
ser.write('\r')
out = ser.read(8)
print out
ser.write(sh)
ser.write('\r')
out = ser.read(8)
print out
#i = 0
#while 1:
#  output = ser.read(1)     # read a byte
#  print "i = %d: %x %c" % (i, ord(output), output )
#  i = i + 1
ser.close()             # close port
