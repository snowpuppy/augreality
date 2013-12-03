/*
 * Filename: cserver.c
 * Author: Thor Smith
 * Purpose: Create a sample c server to serve content
 *          and run commands on request.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "serverComm.h"

// CONSTANTS
#define PORT 7777
#define LISTENQ 100
#define DEBUG 1

// FUNCTIONS
int bindServer(uint16_t port);
void serviceConnections(int fd);
void sendBroadCastData(int connfd);
void sendHeartbeatData(int connfd);

int main(void)
{
  int inetfd = 0;
  // Establish TCP port connection.
  inetfd = bindServer(PORT);
  if (inetfd < 0)
  {
    perror("Error setting up server.\n"); exit(1);
  }
  // Wait for data from clients. (use select)
  serviceConnections(inetfd);
  // Do things in response to receipt of data.
  close(inetfd);
  return 0;
}

int bindServer(uint16_t port)
{
  int ret = 0;
  // connection variables
  //
  int listenfd = 0, optval = 1;
  struct sockaddr_in serveraddr = {0};

  // Start listening on port
  // And create the listening Socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
  {
    fprintf(stderr, "Could not connect to port %d!\n", port); return -1;
  }

  ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));
  if (ret < 0)
  {
    fprintf(stderr, "Could not set options!\n"); return -1;
  }

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(port);

  ret = bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
  if (ret < 0)
  {
    fprintf(stderr, "Unable to bind to port %d!\n", port); return -1;
  }

  ret = listen(listenfd, LISTENQ);
  if (ret < 0)
  {
    fprintf(stderr, "Failed to make a listening socket!\n"); return -1;
  }

  return listenfd;
}

void serviceConnections(int fd)
{
  int rc = 0;
  int clientlen = 0, connfd;
  struct sockaddr_in clientaddr = {0};
  struct hostent *host = NULL;
  // Select variables
  //fd_set rfds;
  //struct timeval tv;

  while(1)
  {
    uint8_t packetType = 0;
    clientlen = sizeof(clientaddr);
    connfd = accept(fd, (struct sockaddr *)&clientaddr, &clientlen);
    host = gethostbyaddr( (const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
#if DEBUG == 1
    printf("Connected to %s\n", host->h_name);
#endif

    // Handle the request here.
    rc = read(connfd, (void *)&packetType, 1);
    switch(packetType)
    {
      case GETBROADCAST:
        sendBroadCastData(connfd);
        break;
      case GETHEARTBEAT:
        sendHeartbeatData(connfd);
        break;
      default:
        break;
    }
    close(connfd);
  }
}

void sendBroadCastData(int connfd)
{
  broadCastInfo_t p = {0};
  p.address = 0x3322;
  p.latitude = 345.22;
  p.longitude = 222.45;
  //htonBroadCastInfo(&p);
  write(connfd, (void *)&p, sizeof(p));
}

void sendHeartbeatData(int connfd)
{
  heartBeatInfo_t p = {0};
  p.id = 0x3322;
  p.x = 22;
  p.y = 32;
  p.pitch = 30;
  p.roll = 90;
  p.yaw = 180;
  //htonHeartBeatInfo(&p);
  write(connfd, (void *)&p, sizeof(p));
}
