CC=g++
CFLAGS=-c -Wall -O3 -I/usr/local/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include -I$(HOME)/3d/pingl/include -I/usr/include/ImageMagick  -DNGL_DEBUG
LDFLAGS=-L/opt/vc/lib -lEGL -lGLESv2 -lbcm_host -L/home/pi/3d/pingl/lib -lNGL -lMagickCore -lMagick++ -lSDL -lSDLmain -L/usr/local/include -lwiringPi
SOURCES=MyGLWindow.cpp main.cpp
OBJECTS=$(SOURCES:%.cpp=%.o)
EXECUTABLE=render

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o $(EXECUTABLE)
