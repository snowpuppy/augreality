#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "threadInterface.h"
#include "packetLib.h"
#include "packets.h"

// CONSTANTS
#define PORT 7777
#define LISTENQ 100
#define DEBUG 1

// GLOBAL VARIABLES
pthread_t tidp;
int quit = 0;

// LOCAL FUNCTIONS
int bindServer(uint16_t port);
void serviceConnections(int fd);
void *threadServer(void *arg);

void _sendGoBack(int fd);
void _sendAccept(int fd);
void _sendStart(int fd);
void _sendEnd(int fd);
void _sendFile(int fd);
void _sendUpdateObjs(int fd);
void _getAlive(int fd);
void _getNumAlive(int fd);
void _getPosition(int fd);
void _getBroadcastLoc(int fd);
void _getNumBroadcast(int fd);
void _getBroadCastIDs(int fd);

// Function: initServer()
// Purpose: starts the server that will
// communicate with the gui and simulation.
int initServer(void)
{
	int ret = 0;
	ret = pthread_create(&tidp, NULL, threadServer, NULL);
	return ret;
}

// Function: stopServer()
// Purpose: stops the server that will
// communicate with the gui and simulation.
void stopServer(void)
{
	// Assert quit signal
	// and join the thread.
	quit = 1;
	pthread_join(tidp, NULL);
}

// Function: threadServer()
// Purpose: starting function for the
// thread.
void *threadServer(void *arg)
{
  int inetfd = 0;
  // Establish TCP port connection.
  inetfd = bindServer(PORT);
  if (inetfd < 0)
  {
    perror("Error setting up server.\n"); exit(1);
  }
  // Wait for data from clients.
  serviceConnections(inetfd);
	// Close socket.
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

	// While the application has not quit, continue
	// to service client requests.
  while(!quit)
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
		// Find out what type of request was sent
    switch(packetType)
    {
      case GETBROADCASTIDS:
				_getBroadCastIDs(connfd);
        break;
      case GETNUMBROADCAST:
				_getNumBroadcast(connfd);
        break;
			case GETBROADCASTLOC:
				_getBroadcastLoc(connfd);
				break;
			case GETPOSITION:
				_getPosition(connfd);
				break;
			case GETNUMALIVE:
				_getNumAlive(connfd);
				break;
			case GETALIVE:
				_getAlive(connfd);
				break;
			case SENDUPDATEOBJS:
				_sendUpdateObjs(connfd);
				break;
			case SENDFILE:
				_sendFile(connfd);
				break;
			case SENDEND:
				_sendEnd(connfd);
				break;
			case SENDSTART:
				_sendStart(connfd);
				break;
			case SENDACCEPT:
				_sendAccept(connfd);
				break;
			case SENDGOBACK:
				_sendGoBack(connfd);
				break;
      default:
        break;
    }
		// Close the client connection.
		// We only service one request
		// per connection. (perhaps
		// inefficient, but it works.)
    close(connfd);
  }
}

void _getBroadCastIDs(int fd)
{
	int rc = 0;
	uint8_t ids[MAXNUMHEADSETS*SIZEOFID];
	int num = getNumBroadCasting();
	// get ids.
	getBroadCastingIDs(ids, num);
	// send the return message.
	rc = write(fd, (void *)ids, num*SIZEOFID);
}
void _getNumBroadcast(int fd)
{
	int rc = 0;
	int num = getNumBroadCasting();
	rc = write(fd, (void *)&num, sizeof(num));
}
void _getBroadcastLoc(int fd)
{
	int rc = 0;
	uint8_t id[SIZEOFID];
	headsetPos_t pos = {0};
	// Read in the id
	rc = read(fd, (void *)id, SIZEOFID);
	if (rc < SIZEOFID) { perror("Error:_getBroadcastLoc: read less than size of id!\n"); }
	// Find the Loc element for this id and send our reply.
	getBroadCastingLoc(&pos, id);
	// Send the position information out.
	rc = write(fd, (void *)&pos, sizeof(pos));
}
void _getPosition(int fd)
{
	int rc = 0;
	uint8_t id[SIZEOFID];
	headsetPos_t pos = {0};
	// Read in the id
	rc = read(fd, (void *)id, SIZEOFID);
	if (rc < SIZEOFID) { perror("Error:_getPosition: read less than size of id!\n"); }
	// Find the Loc element for this id and send our reply.
	getPos(&pos, id);
	// Send the position information out.
	rc = write(fd, (void *)&pos, sizeof(pos));
}
void _getNumAlive(int fd)
{
	int rc = 0;
	int num = 0;
	num = getNumAlive();
	rc = write(fd, (void *)&num, sizeof(num));
}
void _getAlive(int fd)
{
	int rc = 0;
	uint8_t id[SIZEOFID];
	uint16_t alive = 0;
	// Read in the id
	rc = read(fd, (void *)id, SIZEOFID);
	if (rc < SIZEOFID) { perror("Error:_getAlive: read less than size of id!\n"); }
	// Find the Loc element for this id and send our reply.
	alive = getAlive(id);
	// Send the position information out.
	rc = write(fd, (void *)&alive, sizeof(alive));
}
// This function needs to be
// tested thoroughly. It is probably
// the most complicated function second
// only to file transfers.
void _sendUpdateObjs(int fd)
{
	static uint8_t updateNumber = 0;
	int rc = 0;
	uint8_t numObjs = 0;
	uint16_t i = 0;
	updateObjInstance_t objs = {0};
	objInfo_t objInfo = {0};
	// reserve a large buffer because this
	// could be a large update.
	uint8_t buf[5024] = {0};
	// Read in number of objects to update
	rc = read(fd, (void *)&numObjs, sizeof(numObjs));
	if (rc < sizeof(numObjs)) { perror("Error:_sendUpdateObjs: read less than size of numObjs!\n"); }
	// Write header to packet
	addHeader(buf);
	// Write the objs packet.
	objs.packetType = UPDATEOBJINSTANCE;
	objs.numObj = numObjs;
	objs.updateNumber = updateNumber++;
	updateObjInstancePack(&objs, &buf[HEADERSIZE]);
	// Pack in all of the object update information.
	for (i = 0; i < numObjs && i < 150; i++)
	{
		rc = read(fd, (void *)&objInfo, sizeof(objInfo));
		objInfoPack(&objInfo, &buf[i*OBJINFOSIZE + UPDATEOBJINSTANCESIZE + HEADERSIZE]);
	}
	// Now that the bytestream has been populated, it is time to send out wireless.
	rc = writeByteStream(buf, i*OBJINFOSIZE + UPDATEOBJINSTANCESIZE + HEADERSIZE);
}
void _sendFile(int fd)
{
	int rc = 0;
	// Limit the filename to 256 characters (more than reasonable).
	uint8_t filename[256];
	uint8_t filenameSize = 0;

	// Read size of the filename
	rc = read(fd, (void *)&filenameSize, sizeof(filenameSize));
	if (rc < sizeof(filenameSize)) { perror("Error:_sendFile: read less than size of filenameSize!\n"); }
	// Quit if filename is wrong size.
	if (filenameSize >= 256)
	{
		perror("Error:_sendFile: Filename is too long!\n");
		return;
	}
	// Read in filename
	rc = read(fd, (void *)&filename, filenameSize+1);
	if (rc < filenameSize+1) { perror("Error:_sendFile: read less than filenameSize+1!\n"); return;}
	// Send the file on its way!
	sendFile(filename);
}
// The functions below need to be sent to a
// specific headset. Therefore the id of the
// headset to send to will need to be retrieved
// and used to send the information.
void _sendEnd(int fd)
{
	uint8_t buf[ENDSIMULATIONSIZE + HEADERSIZE];
	endSimulation_t p = {0};
	p.packetType = ENDSIMULATION;
	addHeader(buf);
	endSimulationPack(&p, &buf[HEADERSIZE]);
	writeByteStream(buf, ENDSIMULATIONSIZE + HEADERSIZE);
}
void _sendStart(int fd)
{
	uint8_t buf[STARTSIMULATIONSIZE + HEADERSIZE];
	startSimulation_t p = {0};
	p.packetType = STARTSIMULATION;
	addHeader(buf);
	startSimulationPack(&p, &buf[HEADERSIZE]);
	writeByteStream(buf, STARTSIMULATIONSIZE + HEADERSIZE);
}
void _sendAccept(int fd)
{
	uint8_t buf[ACCEPTHEADSETSIZE + HEADERSIZE];
	acceptHeadset_t p = {0};
	p.packetType = ACCEPTHEADSET;
	//p.id = getCcuId();
	// Id of headset needs to be passed to getOrigin
	// so that I know which headset is being accepted
	// and so I can update the origin correctly. :)
	//getOrigin(&p.x, &p.y, /*id of headset */);
	addHeader(buf);
	acceptHeadsetPack(&p, buf);
	writeByteStream(buf, ACCEPTHEADSETSIZE + HEADERSIZE);
}
// This needs to be sent to specific address
// or broadcast as needed...
void _sendGoBack(int fd)
{
	uint8_t buf[GOBACKSIZE + HEADERSIZE];
	goBack_t p = {0};
	p.packetType = GOBACK;
	addHeader(buf);
	goBackPack(&p, &buf[HEADERSIZE]);
	writeByteStream(buf, GOBACKSIZE + HEADERSIZE);
}
