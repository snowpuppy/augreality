all: packets.cpp networking_daemon.cpp packetLib.cpp packetLib.h packets.h threadInterface.cpp threadInterface.h gpsIMUDataThread.cpp gpsIMUDataThread.h
	g++ -o sim packetLib.cpp networking_daemon.cpp packets.cpp threadInterface.cpp gpsIMUDataThread.cpp -lpthread -fpermissive
packets.h: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
packets.cpp: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
