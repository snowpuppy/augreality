all: packets.c packet_daemon.c packetLib.c packetLib.h packets.h threadInterface.c threadInterface.h
	gcc -o sim packetLib.c packet_daemon.c packets.c threadInterface.c -lpthread
packets.h: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
packets.c: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
