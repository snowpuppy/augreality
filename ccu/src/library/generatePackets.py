#!/usr/bin/python

from packetStrings import *

fdh = open("packets.h","w")
fdc = open("packets.c","w")

fdh.write("#ifndef APACKETS_H"
				 "\n#define APACKETS_H"
				 "\n\n//THIS FILE HAS BEEN AUTO"
				 "\n//GENERATED. DO NOT EDIT. FIND"
				 "\n//generatePackets.py INSTEAD"
)

# Setup the header file
fdh.write(str(typesEnum))
for i in packetList:
	fdh.write(str(i))

fdh.write("\n// Packet Functions\n")
for i in packetList:
	fdh.write(i.sizeFunctionPrototype())
	fdh.write(i.packFunctionPrototype())

fdh.write("\n#endif");

# Setup the C functions
fdc.write("#include \"packets.h\"\n")
for i in packetList:
	fdc.write(i.sizeFunction());
	fdc.write(i.packFunction());

fdh.close()
fdc.close()
