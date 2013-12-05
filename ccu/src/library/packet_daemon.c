#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "packets.h"
#include "packetLib.h"
#include "threadInterface.h"

// Other functions.
void printFloatBytes(char *buf);

// Main Function.
int main(void)
{
  int fd = 0;
	int ret = 0;
	uint8_t pac[3];
	uint8_t packetType = 0;
	broadCastPacket_t packet;

  // Create a separate thread which will launch server functions
	ret = initServer();
	if (ret < 0)
	{
		perror("Problem starting thread...\n"); return 1;
	}
	// Wait forever
	while(1)
	{
		// Open the serial port for communication.
		fd = openComPort();
		// detect packet header
		packetType = detectHeader(pac);
		// Process the type of packet.
		switch(packetType)
		{
			case BROADCASTPACKET:
				getBroadCastPacket();
				break;
			case HEARTBEAT:
				getHeartBeatPacket();
				break;
			case CONFIRMUPDATE:
				break;
			default:
				break;
		}
	}
	// Open the serial port for communication.
	//fd = openComPort();
  //sendFile("../sim/sampleFile.tar");
/*
	//res = write(fd,"abcdh",5);
	while (1)
	{
		readBytes(fd,buf,3);
		if (buf[0] == 'P' && buf[1] == 'A' && buf[2] == 'C')
		{
			printf("Recieving packet...\n");
		}
		else
		{
			//printf("Received dead stuff...\n");
			continue;
		}
		readBytes(fd,buf,1);
		if (buf[0] == (char)0)
		{
			printf("Received a broadcastPacket....\n");
		}
		else
		{
			printf("Received a packet of type %d....\n",(int)(buf[0]));
			continue;
		}
		readBytes(fd,buf,2);
		packet.address = *((short *)buf);
		printf("Wireless ID: %X\n", packet.address);
		readBytes(fd,buf,4);
		//printf("lattitude: [0] = %X, [1] = %X, [2] = %X, [3] = %X\n", buf[0], buf[1], buf[2], buf[3]);
		packet.latitude = *((float *)buf);
		//printf("Lattitude: %X\n", packet.latitude);
		//printf("Lattitude: %d\n", packet.latitude);
		printf("Lattitude: sizeoffloat = %d, %f\n", sizeof(float), packet.latitude);//(float)packet.latitude/10000.0);
		readBytes(fd,buf,4);
		//printf("longitude: [0] = %X, [1] = %X, [2] = %X, [3] = %X\n", buf[0], buf[1], buf[2], buf[3]);
		packet.longitude = *((float *)buf);
		//printf("Longitude: %X\n", packet.longitude);
		//printf("Longitude: %d\n", packet.longitude);
		printf("Longitude: %f\n", packet.longitude);//(float)packet.longitude/10000.0);
		readBytes(fd,buf,2);
		crc = *((short *)buf);
		printf("Crc: %X\n", crc);
	}
  */
	return 0;
}

