#!/usr/bin/python
# Python example with serial
# stuff.

import serial

ser = serial.Serial("/dev/ttyUSB0")  # open first serial port
ser.baudrate = 57600
ser.timeout = None
print ser.portstr       # check which port was really used
#ser.write("h0ello")     # write a string
i = 0
newfile = open("output.txt","wb")
count = 0
quit = 0

# PAC sequence detector
byte0 = 0
byte1 = 0
byte2 = 0
while quit == 0:
	byte0 = ser.read(1)
	print byte0, byte1, byte2
	if (byte0 == 'C' and byte1 == 'A' and byte2 == 'P'):
		quit = 1
	byte2 = byte1
	byte1 = byte0

output = ser.read(1)
print "Packet Type = %d" % ( ord(output), )
output = ser.read(4)
print "File size: %s" % (output.encode('hex'), )
count = count + 5
#filesize = ord(output[3])*(2**31) + ord(output[2])*(2**23) + ord(output[1])*(2**15) + ord(output[0])*(2**7)
filesize = int(''.join(reversed(output)).encode('hex'), 16)
print "Filesize =", filesize

count = 0
while count < filesize:
  output = ser.read(1)     # read a byte
  newfile.write(output);
  print "i = %d: %x %c" % (i, ord(output), output )
  i = i + 1
  newfile.flush()
  count = count + 1
ser.close()             # close port
newfile.close()
