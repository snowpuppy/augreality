#ifndef BROADMOD_H
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
#include <ifaddrs.h>

#define DEBUG 1

int bindServer(uint16_t port);
int bindToUdpPort(int port);
int sendBroadCastPacket(char *msg, uint32_t size, int32_t port, int32_t fd);
void sendAcceptPacket(int port);
int checkForNewPackets(int fd1, int fd2);
void getAcceptPacket(int fd);
int wirelessConnection();

#endif
