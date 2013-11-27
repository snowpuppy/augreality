#!/usr/bin/python

from packetStrings import *

fdh = open("packets.h","w")
fdc = open("packets.c","w")

fdh.write("#ifndef APACKETS_H"
				 "\n#define APACKETS_H"
				 "\n#include <stdint.h>"
				 "\n\n//THIS FILE HAS BEEN AUTO"
				 "\n//GENERATED. DO NOT EDIT. FIND"
				 "\n//generatePackets.py INSTEAD"
)

# Setup the header file
fdh.write("\n\n// general shared constants")
fdh.write(addDefines)
# Print size of each struct
fdh.write("\n\n// #defines for size of each packet")
for i in packetList:
	fdh.write(i.printSize())

# Print enum to indicate type of packet
fdh.write(str(typesEnum))
# Print each structure definition
for i in packetList:
	fdh.write(str(i))

# Print prototype for pack function
fdh.write("\n// Packet Functions\n")
for i in packetList:
	fdh.write(i.packFunctionPrototype())
# add additional function prototypes
# shared between headset and ccu
fdh.write(addFuncProtStr)

# end the header file.
fdh.write("\n#endif");

# Setup the C functions
fdc.write("#include \"packets.h\"\n")
for i in packetList:
	fdc.write(i.packFunction());
# add additional function definitions
# shared between headset and ccu
fdc.write(addFuncDefStr)

fdh.close()
fdc.close()
