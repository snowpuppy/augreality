all: spitest.c
	gcc spitest.c -std=gnu99 -o spi -I/usr/local/include -L/usr/local/include -lwiringPi -O2 -Wall
wiringPi: wiringPi/
	git clone git://git.drogon.net/wiringPi
	./wiringPi/build
setup:
	sudo gpio load spi
