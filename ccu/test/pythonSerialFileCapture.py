#!/usr/bin/python
# Python example with serial
# stuff.

import serial

ser = serial.Serial("/dev/ttyUSB0")  # open first serial port
ser.baudrate = 57600
print ser.portstr       # check which port was really used
#ser.write("h0ello")     # write a string
i = 0
newfile = open("output.txt","wb")
count = 0
while count < 8:
	output = ser.read(1)
	count = count + 1
while 1:
  output = ser.read(1)     # read a byte
  newfile.write(output);
  print "i = %d: %x %c" % (i, ord(output), output )
  i = i + 1
  newfile.flush()
ser.close()             # close port
newfile.close()
