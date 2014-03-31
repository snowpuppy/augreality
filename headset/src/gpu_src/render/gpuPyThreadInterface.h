#ifndef GPUPYTHREADINTERFACE_H
#define GPUPYTHREADINTERFACE_H

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

enum gpuApiRequest {
	GETRUNNING = 1,
	GPUQUIT,
	UPDATEOBJECTS
};

class GpuPyThreadInterface
{
	public:
	// Function: initServer()
	// Purpose: starts the server that will
	// communicate with the gui and simulation.
	int initServer(void);
	// Function: stopServer()
	// Purpose: stops the server that will
	// communicate with the gui and simulation.
	void stopServer(void);

	private:
	int bindServer(uint16_t port);
	void serviceConnections(int fd);
	static void *_threadServer(void *This);
	void threadServer();

	// ApiImplementation functions
	// These functions service socket
	// requests on behalf of the requester
	void _getRunning(int fd);
	void _gpuQuit(int fd);
	void _updateObjects(int fd);

	private:
	pthread_t tidp;
	int quit;
};

#endif
