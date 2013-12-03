#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include "serverComm.h"

void ntohBroadCastInfo(broadCastInfo_t *p)
{
  p->address = ntohs(p->address);
  p->latitude = ntohl(p->latitude);
  p->longitude = ntohl(p->longitude);
}

void htonBroadCastInfo(broadCastInfo_t *p)
{
  p->address = htons(p->address);
  p->latitude = htonl(p->latitude);
  p->longitude = htonl(p->longitude);
}

void ntohHeartBeatInfo(heartBeatInfo_t *p)
{
  p->id = ntohs(p->id);
  p->x = ntohl(p->x);
  p->y = ntohl(p->y);
  p->roll = ntohl(p->roll);
  p->pitch = ntohl(p->pitch);
  p->yaw = ntohl(p->yaw);
}

void htonHeartBeatInfo(heartBeatInfo_t *p)
{
  p->id = htons(p->id);
  p->x = htonl(p->x);
  p->y = htonl(p->y);
  p->roll = htonl(p->roll);
  p->pitch = htonl(p->pitch);
  p->yaw = htonl(p->yaw);
}
