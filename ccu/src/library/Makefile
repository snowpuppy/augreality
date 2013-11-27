all: packets.c packet_daemon.c packetLib.c packetLib.h packets.h
	gcc -o sim packetLib.c packet_daemon.c packets.c
packets.h: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
packets.c: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
