all: networking_daemon.cpp packets.cpp packetLib.cpp packetLib.h packets.h threadInterface.cpp threadInterface.h gpsIMUDataThread.cpp gpsIMUDataThread.h
	g++ -o networkDaemon packets.cpp packetLib.cpp networking_daemon.cpp threadInterface.cpp gpsIMUDataThread.cpp -lpthread -fpermissive -lm
packets.h: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
packets.cpp: generatePackets.py packetStrings.py
	./generatePackets.py ./ ./
