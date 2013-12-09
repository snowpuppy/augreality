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
#include "packets.h"

#define GETXBEEDATA 3
#define FLUSHSPIBUFFER 2
#define GETHEADSETDATA 1
#define CHIP_SELECT 1
#define SPI_CLK 1000000
#define OUTFILENAME "output.tar.gz"
#define SENSOR_SIZE 22

MyGLWindow *win;
pthread_mutex_t mut;
objInfo_t g_objList[256];

void getNewFile(uint8_t *buffer, uint8_t bytesRead);
void getObjectUpdateInfo(uint8_t *buffer, uint8_t bytesRead);
void startSimulation(uint8_t *buffer, uint8_t bytesRead);
void endSimulation(uint8_t *buffer, uint8_t bytesRead);
void clearSpiBuffer(void);
void requestXbeeData(void);
void requestHeadsetData(void);
uint8_t getCommandResponse(void);
uint8_t getSpiByte(void);
void getSpiBytes(uint8_t *buf, uint8_t numBytes);
bool running = true;

void exitfunc() {
	delete win;
	SDL_Quit();
	bcm_host_deinit();
}

void readSensorPacket(unsigned char *buf) {
	#define SENSOR_OFFSET (5*sizeof(float))
	pthread_mutex_lock(&mut);
	memcpy(MyGLWindow::buffer(), buf, SENSOR_OFFSET);
	memcpy(MyGLWindow::charbuffer, buf + SENSOR_OFFSET, SENSOR_SIZE - SENSOR_OFFSET);
	pthread_mutex_unlock(&mut);
}

// Function: clearSpiBuffer
// Purpose: This function sends the flush
// buffer command to the headset to indicate
// that it should remove all items from its spi
// buffer.
void clearSpiBuffer(void) {
    char data = FLUSHSPIBUFFER;
	wiringPiSPIDataRW(0, (unsigned char *)&data, 1);
}

void *spiThread(void *arg) 
{
	std::cout << "starting spi thread\n";
	// Buffer for all data
	unsigned char buf[SENSOR_SIZE] = {0};
	unsigned char buffer[256] = {0};
	unsigned char dataSize = 0;

	// Initialize spi.
	wiringPiSetup();
	wiringPiSPISetup(0, SPI_CLK);
	sleep(5);
	// Evict the buffer
  clearSpiBuffer();
	sleep(5);

	while(1) {
		// reset data size to zero.
		dataSize = 0;
		//printf("Requesting Xbee data...\n");
		// Process data from XBee
		while (dataSize == 0)
		{
			// Signal for xbee data.
			requestXbeeData();
			// Get response
			dataSize = getCommandResponse();
			// Get data.
			getSpiBytes(buffer, dataSize);
			// If the response indicated no data
			// was ready, then quit and get headset
			// data. (This should happen a lot).
			if (dataSize == 1 && buffer[0] == 0)
				break;
			printf("Received Xbee packet of type %d\n", (uint32_t)buffer[0]);
			// Find out what kind of packet
			// was sent and process it
			// appropriately.
			switch(buffer[0])
			{
				case LOADSTATICDATA:
					printf("Received file packet.\n");
					getNewFile(buffer, dataSize);
					win->loadConfigFile("config.txt");
					break;
				case UPDATEOBJINSTANCE:
					getObjectUpdateInfo(buffer, dataSize);
					break;
				case STARTSIMULATION:
					startSimulation(buffer, dataSize);
					break;
				case ENDSIMULATION:
					endSimulation(buffer, dataSize);
					break;
				default:
					break;
			}
		}

		// Get headset data!
		requestHeadsetData();
		dataSize = getCommandResponse();
		// verify data size
		if (dataSize != SENSOR_SIZE) {
			fprintf(stderr, "Data size not correct.\n");
			clearSpiBuffer();
			continue;
		}
		usleep(1000);
		getSpiBytes(buf, dataSize);
		readSensorPacket(buf);
		//printf("x: %.03f, y: %.03f, p: %.03f, y: %.03f, r: %.03f\n", *((float*)&buf[0]),*((float*)&buf[4]), *((float*)&buf[8]), *((float*)&buf[12]), *((float*)&buf[16]) );
	}
	std::cout << "finished spi thread\n";
	return NULL;
}

void startSpiThread(void) {
	pthread_t t;
	pthread_create(&t, NULL, spiThread, NULL);
}

int main()
{
  bcm_host_init();
	atexit(exitfunc);

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
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
	startSpiThread();
	win= new MyGLWindow(config);
	
	// loop and process (escape exits)
	while(!win->exit())
	{
		while(!running) {};
		win->processEvents();
		win->paintGL();
	}
}


/**
* @brief Send command to request xbee data.
*
*/
void requestXbeeData(void)
{
	char data = GETXBEEDATA;
	wiringPiSPIDataRW(0, (unsigned char *)&data, 1);
}

/**
* @brief Send command to request xbee data.
*
*/
void requestHeadsetData(void)
{
	char data = GETHEADSETDATA;
	wiringPiSPIDataRW(0, (unsigned char *)&data, 1);
}

/**
* @brief Get number of bytes to read after sending
*				 a command. All requests will return a
*				 response.
*
* @return number of bytes to read.
*/
uint8_t getCommandResponse(void)
{
	uint8_t data = 0;
	// Wait for nonzero data.
	while (data == 0)
	{
		// send command
		wiringPiSPIDataRW(0, &data, 1);
		// sleep
		usleep(1000);
	}
	//printf("SizeOfData: %d\n", (uint32_t)data);
	return data;
}

uint8_t getSpiByte(void)
{
	uint8_t data = 0;
	wiringPiSPIDataRW(0, (unsigned char *)&data, 1);
	return data;
}

void getSpiBytes(uint8_t *buf, uint8_t numBytes)
{
	// Clear out the buffer.
	memset(buf, 0, numBytes);
	// Fill the buffer with data.
	wiringPiSPIDataRW(0, (unsigned char *)buf, numBytes);
}


void endSimulation(uint8_t *buffer, uint8_t bytesRead)
{
	// Set end simulation true.
	printf("Received end simulation request.\n");
	running = false;
	return;
}
void startSimulation(uint8_t *buffer, uint8_t bytesRead)
{
	// Set start simulation true.
	printf("Received start simulation request.\n");
	running = true;
	return;
}
void getObjectUpdateInfo(uint8_t *buffer, uint8_t bytesRead)
{
	// Object list to be updated.
	// g_objList
	uint8_t dataSize = bytesRead;
	uint8_t numObjectsLeft = buffer[1];
	uint16_t i = 0;

	// Note that packetType is NOT in buffer
	// Use buffer[2] because the first packet
	// has 1 byte for numObj and one for updateNumber
	objInfoUnpack(&g_objList[i], &buffer[2]);
	printf("Object[ instId: %d, typeShow: %d, x2: %d, y2: %d]\n", (int)g_objList[i].instId, (int)g_objList[i].typeShow, g_objList[i].x2, g_objList[i].y2);
	printf("Object[ x3: %f, y3: %f, roll: %f, pitch: %f, yaw: %f]\n", g_objList[i].x3, g_objList[i].y3, g_objList[i].roll, g_objList[i].pitch, g_objList[i].yaw);

	// decrement number of objects.
	numObjectsLeft--;
	i++;
	// Read the remaining objects.
	while (numObjectsLeft > 0)
	{
		// Signal for xbee data.
		requestXbeeData();
		// Get response
		dataSize = getCommandResponse();
		// Get data.
		getSpiBytes(buffer, dataSize);
		// If the response indicated no data
		// was ready, then keep trying
		if (dataSize == 1 && buffer[0] == 0)
			continue;

		// pack the next update
		objInfoUnpack(&g_objList[i], &buffer[2]);
		printf("Object[ instId: %d, typeShow: %d, x2: %d, y2: %d]\n", (int)g_objList[i].instId, (int)g_objList[i].typeShow, g_objList[i].x2, g_objList[i].y2);
		printf("Object[ x3: %f, y3: %f, roll: %f, pitch: %f, yaw: %f]\n", g_objList[i].x3, g_objList[i].y3, g_objList[i].roll, g_objList[i].pitch, g_objList[i].yaw);
		// decrement number of objects.
		numObjectsLeft--;
		i++;
	}
	
	int id;
	pthread_mutex_lock(&mut);
	for(i=0; i<256; i++)
	{
		id = g_objList[i].instId;
		win->objects[id].x = g_objList[i].x3;
		win->objects[id].y = g_objList[i].y3;
		win->objects[id].roll = g_objList[i].roll;
		win->objects[id].pitch = g_objList[i].pitch;
		win->objects[id].yaw = g_objList[i].yaw;
		win->objects[id].visible = g_objList[i].x3;
	}
	pthread_mutex_unlock(&mut);
	return;
}

void getNewFile(uint8_t *buffer, uint8_t bytesRead)
{
	// File receipt variables.
	int32_t filesize = 0;
  FILE *filefp = NULL;
	uint8_t dataSize = bytesRead;

	// Open output filename for tarball.
  filefp = fopen(OUTFILENAME, "wb");
  if (filefp == NULL)
  {
    fprintf(stderr, "Could not open file: %s\n", OUTFILENAME);
    exit(1);
  }
	// DEBUG Stuff
  //filefp2 = fopen("out.txt", "w");

	// Write initial data read.
	// Read in type of data and then read
	// the file size
	filesize = *((uint32_t *)&buffer[1]);
	printf("Bytes: %2X:%2X:%2X:%2X:%2X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	printf("Read %d bytes for packet of type %d, filesize = %d\n", (int)dataSize, (uint32_t)buffer[0], filesize);

	// Write first set of bytes to the file.
	fwrite(&buffer[5], 1, (uint32_t)(dataSize-5), filefp);
	// DEBUG Stuff.
	/*
	for (i = 5; i < dataSize; i++)
	{
		fprintf(filefp2, "%02X",buffer[i]);
	}
	fprintf(filefp2, "\n");
	*/

	// decrement remaining bytes
	// to be read.
	filesize -= ((uint32_t)dataSize-5);

	// Get entire file and write
	// to output file.
  while (filesize > 0)
	{
		// Signal for xbee data.
		requestXbeeData();
		// Get response
		dataSize = getCommandResponse();
		// Get data.
		getSpiBytes(buffer, dataSize);
		// If the response indicated no data
		// was ready, then keep trying
		if (dataSize == 1 && buffer[0] == 0)
			continue;

		// Write data to file.
		fwrite(buffer, 1, (uint32_t)dataSize, filefp);
		fflush(filefp);
		// DEBUG Stuff
		/*
			 for (i = 0; i < dataSize; i++)
			 {
			 fprintf(filefp2, "%02X",buffer[i]);
			 }
			 fprintf(filefp2, "\n");
			 fflush(filefp2);
		*/
		printf("Bytes1: %2X:%2X:%2X:%2X:%2X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
		//printf("Bytes2: %2X:%2X:%2X:%2X\n", buffer[dataSize-4], buffer[dataSize-3], buffer[dataSize-2], buffer[dataSize-1]);
		printf("Read %d bytes, %d left\n", (int)dataSize, filesize);

		// decrement file size
		filesize -= (uint32_t)dataSize;
	}
  fclose(filefp);
  system("tar -xzf output.tar.gz; rm output.tar.gz");
}
