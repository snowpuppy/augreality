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
#include <string.h>
#include "threadInterface.h"
#include "gpsIMUDataThread.h"
#include "packetLib.h"
#include "packets.h"

// CONSTANTS
#define PORT 7777
#define LISTENQ 100
#define DEBUG 0

// GLOBAL VARIABLES
pthread_t threadInterfaceTidp;
int threadInterfaceQuit = 0;

// LOCAL FUNCTIONS
int bindServer(uint16_t port);
void serviceConnections(int fd);
void *threadServer(void *arg);

void _sendAccept(int fd);
void _getAccept(int fd); // NEW
void _sendDrop(int fd); // NEW
void _getDrop(int fd); // NEW
void _sendStart(int fd);
void _getStart(int fd); // NEW
void _sendEnd(int fd);
void _getEnd(int fd);  // NEW
void _sendFile(int fd);
void _getReceivedFile(int fd); // NEW
void _sendUpdateObjs(int fd);
void _getUpdateObjs(int connfd); // NEW
void _getAlive(int fd);
void _getNumAlive(int fd);
void _getPosition(int fd);
void _getNumBroadcast(int fd);
void _getBroadCastIDs(int fd);
void _getAcceptedIds(int fd); // NEW
void _getUserPos(int fd);
void _getWifiStatus(int fd);
void _getBatteryStatus(int fd);
void _resetGPSOrigin(int fd);
void _getGPSOrigin(int fd); // NEW
void _setGPSOrigin(int fd); // NEW
void _setHostHeadset(int fd); // NEW
void _getMyId(int fd);        // NEW
void _resetToInit(int fd); // NEW
void _getPosFromGPS(int fd); // NEW

// Function: initServer()
// Purpose: starts the server that will
// communicate with the gui and simulation.
int initServer(void)
{
	int ret = 0;
	ret = pthread_create(&threadInterfaceTidp, NULL, threadServer, NULL);
	return ret;
}

// Function: stopServer()
// Purpose: stops the server that will
// communicate with the gui and simulation.
void stopServer(void)
{
	// Assert threadInterfaceQuit signal
	// and join the thread.
	threadInterfaceQuit = 1;
	pthread_join(threadInterfaceTidp, NULL);
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
  uint32_t clientlen = 0, connfd;
  struct sockaddr_in clientaddr = {0};
  struct hostent *host = NULL;
  // Select variables
  //fd_set rfds;
  //struct timeval tv;

	// While the application has not threadInterfaceQuit, continue
	// to service client requests.
  while(!threadInterfaceQuit)
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
#if DEBUG == 1
		printf("Packet type of %d received.\n", (int)packetType);
#endif
		// Find out what type of request was sent
    switch(packetType)
    {
			case GETUSERPOS:
				_getUserPos(connfd);
        break;
			case GETWIFISTATUS:
				_getWifiStatus(connfd);
        break;
			case GETBATTERYSTATUS:
				_getBatteryStatus(connfd);
        break;
      case GETBROADCASTIDS:
				_getBroadCastIDs(connfd);
        break;
      case GETNUMBROADCAST:
				_getNumBroadcast(connfd);
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
			case RESETGPSORIGIN:
				_resetGPSOrigin(connfd);
        break;
      case SETGPSORIGIN:
        _setGPSOrigin(connfd);
        break;
      case GETGPSORIGIN:
        _getGPSOrigin(connfd);
        break;
      case GETACCEPTEDIDS:
        _getAcceptedIds(connfd);
        break;
      case GETRECEIVEDFILE:
        _getReceivedFile(connfd);
        break;
      case GETEND:
        _getEnd(connfd);
        break;
      case GETSTART:
        _getStart(connfd);
        break;
      case GETDROP:
        _getDrop(connfd);
        break;
      case SENDDROP:
        _sendDrop(connfd);
        break;
      case GETACCEPT:
        _getAccept(connfd);
        break;
      case SETHOSTHEADSET:
        _setHostHeadset(connfd);
        break;
      case GETMYID:
        _getMyId(connfd);
				break;
      case GETUPDATEOBJS:
        _getUpdateObjs(connfd);
        break;
			case RESETTOINIT:
				_resetToInit(fd);
				break;
			case GETPOSFROMGPS:
				_getPosFromGPS(fd);
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

/**
* @brief This call resets the origin for the gps to the
*				 users current gps coordinates.
*
* @param fd
*/
void _resetGPSOrigin(int fd)
{
  printf("Resetting origin.\n");
	localHeadsetPos_t pos;
	getHeadsetPosData(&pos);
  printf("GetPosition returned: %f, %f\n", pos.lat, pos.lon);
	setGPSOrigin(pos.lat,pos.lon);
	return;
}

void _getGPSOrigin(int fd)
{
  int32_t rc = 0;
  float olat = 0, olon = 0;
  getGPSOrigin(&olat,&olon);
  // send the lat then the lon
  rc = write(fd, (void *)&olat, sizeof(olat));
  rc = write(fd, (void *)&olon, sizeof(olon));
	return;
}

void _setGPSOrigin(int fd)
{
  int32_t rc = 0;
  float lat = 0, lon = 0;
  rc = read(fd, (void *)&lat, sizeof(lat));
  rc = read(fd, (void *)&lon, sizeof(lon));
  setGPSOrigin(lat,lon);
  return;
}

/**
* @brief getUserPos - returns the current position
*					of the user by calling into the code
*					that updates the user's position.
*
* @param fd
*/
void _getUserPos(int fd)
{
	int32_t rc = 0;
	localHeadsetPos_t pos = {0};
	getHeadsetPosData(&pos);
	rc = write(fd,(void *)&pos, sizeof(pos));
	return;
}

/**
* @brief getWifiStatus - requests the status of
*				wifi and returns a value that can be used
*				to represent whether the wireless is connected
*				or disconnected.
*
* @param fd
*/
void _getWifiStatus(int fd)
{
	int32_t rc = 0;
	uint32_t id = getMyId();
	uint8_t data = 0;
	// if disconnected, a 0 IP
	if (id == 0)
	{
		data = 0;
	}
	// If connected, then a nonzero IP
	else
	{
		data = 1;
	}
	rc = write(fd, (void *)&data, sizeof(data));
	return;
}

/**
* @brief getBatteryStatus - indicates how
*				full the battery is. This will call
*				into a seperate thread later. For
*				now we'll return a fixed value of
*				255.
*
* @param fd
*/
void _getBatteryStatus(int fd)
{
	int32_t rc = 0;
	uint32_t bVoltage = getHeadsetVoltage();
	// merge the data down to a range from 0 to 10
	uint8_t data = (bVoltage - BATTERY_EMPTY) / ((BATTERY_FULL-BATTERY_EMPTY)/68);
	// send the battery state.
	rc = write(fd, (void *)&data, sizeof(data));
	return;
}

void _getBroadCastIDs(int fd)
{
	int rc = 0;
	uint32_t ids[MAXNUMHEADSETS];
	uint8_t num = (uint8_t)getNumBroadCasting();
	// get ids.
	getBroadCastingIDs(ids, num);
	// send the return message.
	rc = write(fd, (void *)&num, sizeof(num));
	rc = write(fd, (void *)ids, num*sizeof(uint32_t));
	return;
}

void _getNumBroadcast(int fd)
{
	int rc = 0;
	uint8_t num = (uint8_t) getNumBroadCasting();
	rc = write(fd, (void *)&num, sizeof(num));
	return;
}

void _getAcceptedIds(int fd)
{
  int rc = 0;
  uint32_t ids[MAXNUMHEADSETS];
  uint8_t num = (uint8_t)getNumAlive();
  // get ids
  getAliveIDs(ids,num);
  // send the return message
  rc = write(fd, (void *)&num, sizeof(num));
  rc = write(fd, (void *)ids, num*sizeof(uint32_t));
  return;
}

void _getPosition(int fd)
{
  int rc = 0;
  uint32_t id = 0;
  headsetPos_t pos = {0};
  // Get the id needed.
  rc = read(fd, (void *)&id, sizeof(id));
  // Get the position of the id
  getPos(&pos, id);
  // Send the position back.
  rc = write(fd, (void *)&pos, sizeof(pos));
	return;
}
void _getNumAlive(int fd)
{
  int rc = 0;
  uint8_t numAlive = 0;
  numAlive = (uint8_t)getNumAlive();
  rc = write(fd, (void *)&numAlive, sizeof(numAlive));
	return;
}
void _getAlive(int fd)
{
  int rc = 0;
  uint8_t alive = 0;
  uint32_t id = 0;
  // Get the id needed.
  rc = read(fd, (void *)&id, sizeof(id));
  alive = (uint8_t)getAlive(id);
  rc = write(fd, (void *)&alive, sizeof(alive));
	return;
}
// This function needs to be
// tested thoroughly. It is probably
// the most complicated function second
// only to file transfers.
void _sendUpdateObjs(int fd)
{
	objInfo_t updateObjsList[2048];
	uint32_t numObjs;
	int32_t rc = 0;
	int32_t i = 0;

	// Read in the number of objects to retrieve
	rc = read(fd, (void *)&numObjs, sizeof(numObjs));
	// Read in each object.
	for (i = 0; i < numObjs && i < 2048; i++)
	{
		rc = read(fd, (void *)&updateObjsList[i], sizeof(objInfo_t));
	}

	// Send the objects to be updated.
	updateObjs(updateObjsList, numObjs);
	
	return;
}

void _getUpdateObjs(int fd)
{
	std::vector<objInfo_t> myObjs;
	uint32_t numObjects = 0;
	getUpdateObjs(myObjs);
	numObjects = myObjs.size();
	uint32_t i = 0;
	int32_t rc = 0;

	printf("Returning %d objs.\n", numObjects);
	// write number of objects.
	rc = write(fd, (void *)&numObjects, sizeof(numObjects));
	// write each object.
	for (i = 0; i < numObjects; i++)
	{
		rc = write(fd, (void *)&myObjs[i], sizeof(objInfo_t));
	}
	
  return;
}

void _getReceivedFile(int fd)
{
  uint8_t received = getFileReceived();
  int32_t rc = 0;
  char filename[256];
  rc = write(fd, (void *)&received, sizeof(received));
  if (received > 0)
  {
    getReceivedFile(filename, 256);
    received = strlen(filename);
    rc = write(fd, (void *)&received, sizeof(received));
    rc = write(fd, (void *)filename, strlen(filename));
  }
  return;
}

void _sendFile(int fd)
{
	int rc = 0;
	// Limit the filename to 256 characters (more than reasonable).
	uint8_t filename[256];
	uint8_t filenameSize = 0;
	uint32_t id = 0;

	// Read in the id to send it to.
	rc = read(fd, (void *)&id, sizeof(id));
	if (rc < 0)
	{
		perror("Error: sendFile could not read id.\n");
		return;
	}
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
	rc = read(fd, (void *)&filename, filenameSize);
	if (rc < filenameSize) { perror("Error:_sendFile: read less than filenameSize!\n"); return;}
	filename[filenameSize] = '\0';
	// Send the file on its way!
	sendFile((char *)filename, id);
}

void _sendStart(int fd)
{
  startSimulation();
	return;
}

void _getStart(int fd)
{
  int32_t rc = 0;
  uint8_t started = 0;
  rc = getState();
  // Find out if we entered simulation
  // state.
  if (rc == SIMULATION)
  {
    // If so, then we started the simulation.
    started = 1;
  }
  rc = write(fd, (void *)&started, sizeof(started));
  return;
}

// The functions below need to be sent to a
// specific headset. Therefore the id of the
// headset to send to will need to be retrieved
// and used to send the information.
void _sendEnd(int fd)
{
  int32_t rc = 0;
  uint32_t id = 0;
  // Get the id needed.
  rc = read(fd, (void *)&id, sizeof(id));
  if (rc > 0)
  {
    endSimulationID(id);
  }
	return;
}

void _getEnd(int fd)
{
  int32_t rc = 0;
  uint8_t ended = 0;
  rc = getState();
  // Find out if we entered simulation
  // state.
  if (rc == INIT)
  {
    // If so, then we ended the simulation.
    ended = 1;
  }
  rc = write(fd, (void *)&ended, sizeof(ended));
  return;
}

void _sendAccept(int fd)
{
  int32_t rc = 0;
  uint32_t id = 0;
  uint8_t status = 0;
  // Get the id needed.
  rc = read(fd, (void *)&id, sizeof(id));
	printf("Sending accept to id: %u\n rc = %d", id, rc);
  if (rc >= 0)
  {
    rc = acceptID(id);
  }
  // Change rc to reflect the 
  if (rc >= 0)
  {
    status = 1;
  }
  // return whether the call succeeded or failed.
  rc = write(fd, (void *)&status, sizeof(status));
	return;
}

void _getAccept(int fd)
{
  int32_t rc = 0;
  uint8_t accepted = 0;
  rc = getState();
  // Find out if we entered accepted
  // state.
  if (rc == ACCEPTED)
  {
    // If so, then we were accepted.
    accepted = 1;
  }
  rc = write(fd, (void *)&accepted, sizeof(accepted));
  return;
}

void _sendDrop(int fd)
{
  int32_t rc = 0;
  uint32_t id = 0;
  // Get the id needed.
  rc = read(fd, (void *)&id, sizeof(id));
  sendDropId(id);
  return;
}

void _getDrop(int fd)
{
  int32_t rc = 0;
  uint8_t dropped = 0;
  rc = getState();
  // Find out if we entered simulation
  // state.
  if (rc == INIT)
  {
    // If so, then we dropped the simulation.
    dropped = 1;
  }
  rc = write(fd, (void *)&dropped, sizeof(dropped));
  return;
}

void _setHostHeadset(int fd)
{
  int32_t rc = 0;
  uint8_t host = 0;
  rc = read(fd, (void *)&host, sizeof(host));
  setHostHeadset(host);
  return;
}

void _getMyId(int fd)
{
  int32_t rc = 0;
  uint32_t id = 0;
  id = getMyId();
  rc = write(fd, (void *)&id, sizeof(id));
	return;
}

void _resetToInit(int fd)
{
	int32_t rc = 0;
	setState(INIT);
	return;
}

void _getPosFromGPS(int fd)
{
	float lat = 0.0f, lon = 0.0f;
	float x3 = 0.0f, y3 = 0.0f;
	int32_t rc = 0;
	printf("Getting position for coordinates.\n");
	// Read in GPS coordinates.
  rc = read(fd, (void *)&lat, sizeof(lat));
  rc = read(fd, (void *)&lon, sizeof(lon));
	printf("lat = %2.2f, lon = %2.2f\n", lat, lon);
	// Translate to x and y coordinates.
	getPosFromGPS(lat,lon,&x3,&y3);
	printf("x3 = %2.2f, y3 = %2.2f\n", x3, y3);
	printf("Sending data back.\n");
	printf("Sending data back %d %d.\n", sizeof(x3), sizeof(y3));
	// Send back the results.
  rc = write(fd, (void *)&x3, sizeof(x3));
	if (rc < sizeof(x3))
		perror("_getPosFromGPS: Couldn't send all of x3!\n");
	else
		printf("Sent back %d bytes.\n", sizeof(x3));
  rc = write(fd, (void *)&y3, sizeof(y3));
	if (rc < sizeof(y3))
		perror("_getPosFromGPS: Couldn't send all of y!\n");
	else
		printf("Sent back %d bytes.\n", sizeof(y3));
	return;
}
