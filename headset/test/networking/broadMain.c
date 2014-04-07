/*
** broadcaster.c -- a datagram "client" like talker.c, except
**                  this one can broadcast
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "broadMod.h"

#define SERVERPORT 4950    // the port users will be connecting to
#define TCPSERVERPORT 4951

int main(int argc, char *argv[])
{
    int sockfd = 0, tcpfd = 0, ret = 0;
    int numbytes = 0;
		char msg[] = "hello There.";
		int myserver = 0;

		if (argc != 2)
		{
			printf("Usage: %s <server/client>\n", argv[0]);
			exit(1);
		}
		if ( !(strcmp(argv[1],"server") == 0 || strcmp(argv[1],"client") == 0) )
		{
			printf("Must specify either server or client");
			exit(1);
		}
		if (strcmp(argv[1],"server") == 0)
		{
			myserver = 1;
			printf("Acting as server.\n");
		}

		// Find my ip and netmask
		wirelessConnection();

		// set up the sockets.
    sockfd = bindToUdpPort(SERVERPORT);
		if (sockfd < 0)
		{
        perror("socket\n");
        exit(1);
    }
		tcpfd = bindServer(TCPSERVERPORT);
		if (sockfd < 0)
		{
			perror("socket\n");
			exit(1);
		}
		while(1)
		{
			ret = checkForNewPackets(sockfd,tcpfd);
			if (ret == 1)
			{
				printf("Received a new udp packet.\n");
				getBroadCastPacket(sockfd);
			}
			else if (ret == 2)
			{
				printf("Received a new tcp packet.\n");
				getAcceptPacket(tcpfd);
			}

			ret = sendBroadCastPacket(msg, strlen(msg), SERVERPORT, sockfd);
			if (ret < 0)
			{
				perror("Could not send message!\n");
			}

			printf("myserver: %d\n", myserver);
			if (myserver == 0)
			{
				printf("sending accept packet.\n");
				sendAcceptPacket(TCPSERVERPORT);
			}
			
		}

    // this call is what allows broadcast packets to be sent:
    close(sockfd);

    return 0;
}
