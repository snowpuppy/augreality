#!/usr/bin/python
# Python example with serial
# stuff.

import serial

ser = serial.Serial("/dev/ttyUSB0")  # open first serial port
ser.baudrate = 57600
print ser.portstr       # check which port was really used
ser.write("h0ello")     # write a string
output = ser.read(6)     # read a byte
print output
ser.close()             # close port
