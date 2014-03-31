/*
 * Filename: cclient.c
 * Author: Thor Smith
 * Purpose: Create a sample c client to retrieve content
 *          and send commands
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

// CONSTANTS
#define PORT 7778

// FUNCTIONS
int connectToServer(int port);

int main(void)
{
	int fd = 0;
	int rc = 0;
	char msg = 1;
	char ret = 0;
	fd = connectToServer(PORT);
  rc = write(fd, (void *)&msg, sizeof(msg));
	rc = read(fd,(void *)&ret, sizeof(ret));
	if (rc < 0)
	{
		printf("Error reading from socket!\n");
	}
	printf("Read %d\n", ret);
	close(fd);
  return 0;
}

// Connects to INADDR_LOOPBACK
int connectToServer(int port)
{
  int fd = 0;
	int rc = 0;
  struct sockaddr_in sockAddr = {0};

  // Fill out connection structure.
  sockAddr.sin_port = htons(port);
  sockAddr.sin_family = AF_INET;
  
  sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  // Create a socket.
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if ( fd < 0 )
  {
		printf("Error creating socket!\n");
    return -1;
  }

  //Connect to server
  rc = connect(fd, (const struct sockaddr *) &sockAddr, sizeof(sockAddr) );
  if ( rc < 0 )
  {
		printf("Error connecting to port %d!\n", port);
    return -1;
  }
	return fd;
}
