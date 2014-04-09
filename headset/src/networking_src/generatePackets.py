#!/usr/bin/python

import sys
from packetStrings import *

if (len(sys.argv) != 3):
	raise Exception("Usage: %s [path/to/h/] [path/to/c/]\n" % (sys.argv[0]))

fdh = open(sys.argv[1] + "packets.h","w")
fdc = open(sys.argv[2] + "packets.cpp","w")

fdh.write("#ifndef APACKETS_H"
				 "\n#define APACKETS_H"
				 "\n#include <stdint.h>"
				 "\n#include <arpa/inet.h>"
				 "\n\n//THIS FILE HAS BEEN AUTO"
				 "\n//GENERATED. DO NOT EDIT. FIND"
				 "\n//generatePackets.py INSTEAD"
)

# Setup the header file
fdh.write("\n\n// general shared constants")
fdh.write(addDefines)

# Print enum to indicate type of packet
fdh.write(str(typesEnum))
# Print each structure definition
for i in packetList:
	fdh.write(str(i))

# Print prototype for pack function
fdh.write("\n// Packet Functions\n")
for i in packetList:
	fdh.write(i.htonFunctionPrototype())
	fdh.write(i.ntohFunctionPrototype())
# add additional function prototypes
# shared between headset and ccu
fdh.write(addFuncProtStr)

# end the header file.
fdh.write("\n#endif");

# Setup the C functions
fdc.write("#include \"packets.h\"\n")
for i in packetList:
	fdc.write(i.htonFunction());
	fdc.write(i.ntohFunction());
# add additional function definitions
# shared between headset and ccu
fdc.write(addFuncDefStr)

fdh.close()
fdc.close()
