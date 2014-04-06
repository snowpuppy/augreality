
Target := render
Sources := GameObject.cpp SensorReader.cpp gpuPyThreadInterface.cpp main.cpp
IrrlichtInclude := /usr/local/include/
IrrlichtLib := /usr/local/lib/
BinPath = ./

USERCPPFLAGS =
USERCXXFLAGS = -O3 -ffast-math
#USERCXXFLAGS = -g -Wall
USERLDFLAGS =

CPPFLAGS = -I$(IrrlichtInclude) -I/usr/X11R6/include $(USERCPPFLAGS)
CXXFLAGS = $(USERCXXFLAGS)
LDFLAGS = $(USERLDFLAGS)

#default target is Linux
all: all_linux

# target specific settings
all_linux all_win32 static_win32: LDFLAGS += -L$(IrrlichtLib) -lIrrlicht
all_linux: LDFLAGS += -L/usr/X11R6/lib$(LIBSELECT) -lGL -lXxf86vm -lXext -lX11 -lXcursor -lpthread
all_linux clean_linux: SYSTEM=Linux

DESTPATH = $(BinPath)/$(Target)$(SUF)

all_linux :
	$(warning Building...)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(Sources) -o $(DESTPATH) $(LDFLAGS)

clean:
	$(warning Cleaning...)
	@$(RM) $(DESTPATH)
	
format:
	astyle --style=attach --indent=spaces=4 *.cpp *.h

.PHONY: all  clean  

#multilib handling
ifeq ($(HOSTTYPE), x86_64)
LIBSELECT=64
endif
