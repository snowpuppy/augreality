CC=g++
CFLAGS=-c -Wall -Werror=implicit-function-declaration -Wno-strict-aliasing -O3 -I/usr/local/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include -Ipingl/include -I/usr/include/ImageMagick  -DNGL_DEBUG -I/opt/vc/include/interface/vmcs_host/linux/
LDFLAGS=-L/opt/vc/lib -lEGL -lGLESv2 -lbcm_host -Lpingl/lib -lNGL -lMagickCore -lMagick++ -lSDL -lSDLmain -L/usr/local/include -lwiringPi
SOURCES=MyGLWindow.cpp gameobject.cpp main.cpp packets.c
OBJECTS=$(SOURCES:%.cpp=%.o)
EXECUTABLE=render

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) packets.h packets.c pingl/ wiringPi/
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o $(EXECUTABLE) packets.h packets.c

packets.h: ../../../../ccu/src/library/generatePackets.py ../../../../ccu/src/library/packetStrings.py
	../../../../ccu/src/library/generatePackets.py ./ ./
packets.c: ../../../../ccu/src/library/generatePackets.py ../../../../ccu/src/library/packetStrings.py
	../../../../ccu/src/library/generatePackets.py ./ ./

pingl:
	wget http://pingl.googlecode.com/files/pingl.tgz
	tar -xzf pingl.tgz
	rm pingl.tgz
	
wiringPi: wiringPi/
	git clone git://git.drogon.net/wiringPi
	./wiringPi/build
