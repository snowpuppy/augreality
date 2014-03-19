all: packets.cpp networking_daemon.cpp packets.h gpsIMUDataThread.cpp gpsIMUDataThread.h
	g++ -o netDaemon networking_daemon.cpp gpsIMUDataThread.cpp -lpthread
packets.h: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
packets.cpp: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
