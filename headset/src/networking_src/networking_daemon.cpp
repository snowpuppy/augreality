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
#include "gpsIMUDataThread.h"

// Other functions.
void printFloatBytes(char *buf);

// Main Function.
int main(void)
{
  int udpFd = 0, tcpFd = 0, connFd = 0;
	uint32_t clientAddr = 0;
	int ret = 0;
	uint8_t pac[3];
	uint8_t packetType = 0;

	// Initialize networking stuff.
	ret = wirelessConnection();
	if (ret < 0)
	{
		perror("Problem aquiring wireless network ip!\n");
	}
	udpFd = bindToUdpPort(DEFAULT_UDP_PORT);
	if (udpFd < 0)
	{
		perror("Problem binding to udp port!\n");
	}
	tcpFd = bindToTcpServer(DEFAULT_TCP_PORT);
	if (tcpFd < 0)
	{
		perror("Problem binding to tcp port!\n");
	}

	// broadCastPacket_t packet;
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
		/*
		 *			When the networking code is
		 *			finished. There will be a tcp
		 *			and a udp port used for
		 *			communication. There will be
		 *			a distinction between the operation
		 *			of the networking for a client headset
		 *			vs a server headset.
		 *			I will want to peek the first byte to
		 *			find out what kind of packet was sent.
		 */

		// Wait for packet.
		ret = checkForNewPackets(udpFd, tcpFd);
		//printf("Ret = %d.\n", ret);
		// Find out what packet was received.
		switch (ret)
		{
		case 1: // UDP Packet received.
			// detect packet header
			packetType = detectUdpType(udpFd);
			break;
		case 2: // TCP Packet received
							// If headsets send messages
							// to each other at the same time
							// then this program will hang.
			packetType = detectTcpType(tcpFd, &connFd, &clientAddr);
			break;
		case 0: // We timed out.
							// An update will be sent.
							// Sending an update doesn't
							// change state.
			sendUpdatePacket(udpFd);
			break;
		default:
				break;
		}
		// Processing a packet may change
		// state.
		processPacket(udpFd, tcpFd, connFd, clientAddr, ret, packetType);
	}
	return 0;
}
