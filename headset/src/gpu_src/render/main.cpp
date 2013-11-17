/*
  Copyright (C) 2012 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received m_a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <cstdlib>
#include "MyGLWindow.h"
#include "bcm_host.h"
#include <SDL/SDL.h>


MyGLWindow *win;
pthread_mutex_t mut;

void exitfunc()
{
delete win;
SDL_Quit();
bcm_host_deinit();
}


void readSensorPacket(unsigned char *buf) {
	pthread_mutex_lock(&mut);
	memcpy(MyGLWindow::buffer(), buf, SENSOR_SIZE-2);
	memcpy(MyGLWindow::charbuffer, buf, 2);
	pthread_mutex_unlock(&mut);
}

void *spiThread(void *arg) {
	// Buffer for all data
	unsigned char buf[SENSOR_SIZE] = {0};

	while(1) {
		//lock mutex
		unsigned char dataSize = 0;

		// Signal a byte to read.
		buf[0] = 1;
		wiringPiSPIDataRW(0, (unsigned char *)buf,1);
		
		//read sensor data from SPI
		while (dataSize == 0) {
			//buf[0] = SENSOR_PACKET;
			memset(buf,0,SENSOR_SIZE);
			wiringPiSPIDataRW(0, (unsigned char *)buf,1);
			dataSize = buf[0];
			usleep(1000);
		}
		if (dataSize != SENSOR_SIZE) {
			fprintf(stderr, "Data size not correct.\n");
		}
		usleep(1000);
		wiringPiSPIDataRW(0, buf, SENSOR_SIZE);
		readSensorPacket(buf);

		printf("x: %.03f, y: %.03f, p: %.03f, y: %.03f, r: %.03f\n", *((float*)&buf[0]),*((float*)&buf[4]), *((float*)&buf[8]), *((float*)&buf[12]), *((float*)&buf[16]) );
	}
	return NULL;
}

void startSpiThread(pthread_t t) {
	pthread_create(&t, NULL, spiThread, NULL);
}

int main()
{
  bcm_host_init();
	atexit(exitfunc);

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
      std::cerr<<"Unable to init SDL: "<<SDL_GetError()<<"\n";
      exit(EXIT_FAILURE);
  }



	SDL_Surface* myVideoSurface = SDL_SetVideoMode(0,0, 32,  SDL_SWSURFACE);
	// Print out some information about the video surface
	if (myVideoSurface != NULL) {
       std::cout << "The current video surface bits per pixel is " << (int)myVideoSurface->format->BitsPerPixel << std::endl;
       }
	// here I create a config with RGB bit size 5,6,5 and no alpha
	ngl::EGLconfig *config = new ngl::EGLconfig();
	config->setRGBA(8,8,8,8);
	// set the depth buffer
	config->setDepth(24);
	
	// now create a new window using the default config
	pthread_t t;
	wiringPiSetup();
	wiringPiSPISetup(0, 4000000);
	startSpiThread(t);

	win= new MyGLWindow(config);

	
	// loop and process (escape exits)
	while(1)
	{
		win->processEvents();
		win->paintGL();
		if(win->exit()==true)
			break;
	}
}

