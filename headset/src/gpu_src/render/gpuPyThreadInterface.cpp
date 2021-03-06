#include "gpuPyThreadInterface.h"
#include <iostream>
// CONSTANTS
#define PORT 7778
#define LISTENQ 100
#define DEBUG 1

// GLOBAL VARIABLES

// LOCAL FUNCTIONS

// Function: initServer()
// Purpose: starts the server that will
// communicate with the gui and simulation.
int GpuPyThreadInterface::initServer(volatile bool *quitflag)
{
	tidp = 0;
	quit = 0;
	int ret = 0;
	realQuit = quitflag;
	ret = pthread_create(&tidp, NULL, GpuPyThreadInterface::_threadServer, this);
	return ret;
}

// Function: stopServer()
// Purpose: stops the server that will
// communicate with the gui and simulation.
void GpuPyThreadInterface::stopServer(void)
{
	// Assert quit signal
	// and join the thread.
	quit = 1;
	pthread_join(tidp, NULL);
}

/**
* @brief Dummy function to start actual thread.
*
* @param This - pointer to class running thread.
*
* @return nothing.
*/
void *GpuPyThreadInterface::_threadServer(void *This)
{
	((GpuPyThreadInterface *)This)->threadServer();
}

// Function: threadServer()
// Purpose: starting function for the
// thread.
void GpuPyThreadInterface::threadServer()
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
}

int GpuPyThreadInterface::bindServer(uint16_t port)
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

void GpuPyThreadInterface::serviceConnections(int fd)
{
  int rc = 0;
  uint32_t clientlen = 0, connfd;
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
    //printf("Connected to %s\n", host->h_name);
#endif
    // Handle the request here.
    rc = read(connfd, (void *)&packetType, 1);
		//printf("Packet type of %d received.\n", (int)packetType);
		// Find out what type of request was sent
    switch(packetType)
    {
			case GETRUNNING:
				_getRunning(connfd);
				break;
			case GPUQUIT:
				_gpuQuit(connfd);
				break;
			case UPDATEOBJECTS:
				_updateObjects(connfd);
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
* @brief getRunning - returns status of rendering
*				code to determine if it has reached a certain
*				point in initialization.
*
*				TODO: expand to call an internal
*				function.
*
* @param fd
*/
void GpuPyThreadInterface::_getRunning(int fd)
{
	int32_t rc = 0;
	// Send a 1 to indicate it is running.
	// TODO: set data to actual state of GPU process.
	uint8_t data = 1;
	rc = write(fd, (void *)&data, sizeof(data));
}

/**
* @brief gpuQuit - Tells the GPU code to stop
*									running and return because
*								  the simulation is over.
*
* @param fd
*/
void GpuPyThreadInterface::_gpuQuit(int fd)
{
	// Send GPUQUIT signal somehow
	// Don't need to send anything back.
	*realQuit = true;
	std::cout << "GPU quit signal\n";
	return;
}

/**
* @brief updateObjects - updates the list of
*										3d objects in memory based
*										on the information sent by
*									  the calling process.
*
* @param fd
*/
void GpuPyThreadInterface::_updateObjects(int fd)
{
	int32_t rc = 0, i = 0;
	uint32_t numObjs = 0;
	objInfo_t *objs = NULL;
	// Read in the number of objects to read
	rc = read(fd, (void *)&numObjs, sizeof(numObjs));
	if (rc < 0)
	{
		printf("gpuThreadInterface: Error reading numobjects.\n");
		return;
	}
	//printf("Received %d objects\n", numObjs);
	objs = new objInfo_t[numObjs];
	// check if objs is null...
	// Read in each object.
	for (i = 0; i < numObjs; i++)
	{
		//printf("Reading in obj %d\n", i);
		rc = read(fd, (void *)&objs[i], sizeof(objInfo_t));
		if (rc < 0)
		{
			printf("gpuThreadInterface: Error reading in objects.\n");
			delete[] objs;
			return;
		}
		//printf("instId: %d, typeShow: %d, x2: %d, y2: %d, x3: %f, y3: %f, roll: %f, pitch: %f, yaw: %f\n", objs[i].instId, objs[i].typeShow, objs[i].x2, objs[i].y2, objs[i].x3, objs[i].y3, objs[i].roll, objs[i].pitch, objs[i].yaw);
		//printf("Sizeof obj: %d\n", sizeof(objInfo_t));
	}
	// Call a function that will copy these objects
	updateObjects(objs,numObjs);
	// to the gpu's objects.
	delete[] objs;
}
