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
	// Open the serial port for communication.
	fd = openComPort();
	// Wait forever
	while(1)
	{
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
	return 0;
}

