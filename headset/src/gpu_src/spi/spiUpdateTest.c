#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "packets.h"
#include <vector>

#define GETXBEEDATA 3
#define FLUSHSPIBUFFER 2
#define GETHEADSETDATA 1
#define CHIP_SELECT 1
#define SPI_CLK 1000000
#define OUTOUTFILENAME "output.txt"
#define SENSOR_SIZE 22

// Global variables
objInfo_t g_objList[256];

// Functions
void clearSpiBuffer(void);
void spiThread(void);
void requestXbeeData(void);
void requestHeadsetData(void);
uint8_t getCommandResponse(void);
uint8_t getSpiByte(void);
void getSpiBytes(uint8_t *buf, uint8_t numBytes);

int main(void) {
	spiThread();
	return 0;
}

// Function: clearSpiBuffer
// Purpose: This function sends the flush
// buffer command to the headset to indicate
// that it should remove all items from its spi
// buffer.
void clearSpiBuffer(void)
{
	char data = FLUSHSPIBUFFER;
	wiringPiSPIDataRW(0, (unsigned char *)&data, 1);
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
	return data;
}

uint8_t getSpiByte(void)
{
	uint8_t data = 0;
	wiringPiSPIDataRW(0, (unsigned char *)&data, 1);
}

void getSpiBytes(uint8_t *buf, uint8_t numBytes)
{
	// Clear out the buffer.
	memset(buffer, 0, numBytes);
	// Fill the buffer with data.
	wiringPiSPIDataRW(0, (unsigned char *)buf, 1);
}

void spiThread(void)
{
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

		// Process data from XBee
		while (dataSize != 0)
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
			// Find out what kind of packet
			// was sent and process it
			// appropriately.
			switch(buffer[0])
			{
				case LOADSTATICDATA:
					getNewFile(buffer, dataSize);
					break;
				case UPDATEOBJINSTANCE:
					getObjectUpdateInfo();
					break;
				case STARTSIMULATION:
					startSimulation();
					break;
				case ENDSIMULATION:
					endSimulation();
					break;
				default:
					break;
			}
		}

		// Get headset data!
		dataSize = requestHeadsetData();
		// verify data size
		if (dataSize != SENSOR_SIZE) {
			fprintf(stderr, "Data size not correct.\n");
		}
		usleep(1000);
		getSpiBytes(buf, dataSize);
		//readSensorPacket(buf);
		printf("x: %.03f, y: %.03f, p: %.03f, y: %.03f, r: %.03f\n", *((float*)&buf[0]),*((float*)&buf[4]), *((float*)&buf[8]), *((float*)&buf[12]), *((float*)&buf[16]) );
	}
	return NULL;
}

void endSimulation(uint8_t *buffer, uint8_t bytesRead)
{
	// Set end simulation true.
	printf("Received end simulation request.\n");
	return;
}
void startSimulation(uint8_t *buffer, uint8_t bytesRead)
{
	// Set start simulation true.
	printf("Received start simulation request.\n");
	return;
}
void getObjectUpdateInfo(uint8_t *buffer, uint8_t bytesRead)
{
	// Object list to be updated.
	// g_objList
	uint8_t numObjectsLeft = buffer[0];
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
}
