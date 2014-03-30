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
//#include "packetLib.h"
#include "threadInterface.h"
#include "gpsIMUDataThread.h"

// Other functions.
void printFloatBytes(char *buf);

// Main Function.
int main(void)
{
  int fd = 0;
	int ret = 0;
	uint8_t pac[3];
	uint8_t packetType = 0;
	//broadCastPacket_t packet;
  // Create a separate thread which will launch server functions
	ret = initServer();
	if (ret < 0)
	{
		perror("Problem starting thread...\n"); return 1;
	}

	// Create a thread to handle imu and gps data
	ret = initGPSIMUServer();
	if (ret < 0)
	{
		perror("Problem starting GPS/IMU thread...\n"); return 1;
	}

	// Open the server port and listen for connections.
	// Wait forever
	while(1)
	{
		/* TODO: This will become relevant
		 *			when the networking code is
		 *			finished. There will be a tcp
		 *			and a udp port used for
		 *			communication. There will be
		 *			a distinction between the operation
		 *			of the networking for a client headset
		 *			vs a server headset.
		 *			I will want to peek the first byte to
		 *			find out what kind of packet was sent.
		 */

		/*
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
		*/
	}
	return 0;
}

