#include "broadMod.h"

// GLOBAL VARIABLES
struct timeval tv;
uint32_t g_addr = 0;
uint32_t g_myIp = 0;
uint32_t g_myMask = 0;

// update every 50 milliseconds
#define UPDATE_INTERVAL 50000
#define LISTENQ 100

int bindToUdpPort(int port)
{
  // Variables.
  int socketId = 0;
  struct sockaddr_in serveraddr = {0};
  int ret = 0;
	int broadcastVal = 1;

  // Open the socket.
  socketId = socket(AF_INET, SOCK_DGRAM, 0);
  if ( socketId < 0 )
  {
    return -1;
  }

	// Make it so I can broadcast on this socket.
	ret = setsockopt(socketId, SOL_SOCKET, SO_BROADCAST, &broadcastVal, sizeof(broadcastVal) );
	if (ret == -1)
	{
		perror("setsockopt(SO_BROADCAST)");
		return -1;
	}

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(port);
  ret = bind(socketId, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));

  if (ret < 0)
  {
    fprintf(stderr, "Unable to bind to udp port %d!\n", port);
    return -1;
  }
  else
  {
#if DEBUG
    printf("Bound to udp port %d!\n", port);
#endif
  }

  return socketId;
}

int sendBroadCastPacket(char *msg, uint32_t size, int32_t port, int32_t fd)
{
  // Variables.
  int rc = 0;
	int bytesSent = 0;
  struct sockaddr_in clientaddr = {0};
  struct hostent *hp = NULL;

  // Fill out connection structure.
  clientaddr.sin_port = htons(port);
  clientaddr.sin_family = AF_INET;
  
	// Broadcast this message!
	clientaddr.sin_addr.s_addr = htonl(g_myIp | (~g_myMask)); // INADDR_BROADCAST

  //Connect to server
  bytesSent = sendto(fd, msg, size, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr) );
  if ( bytesSent < 0 )
  {
    fprintf(stderr, "Could not connect to server.\n");
    return -1;
  }
  else
  {
#if DEBUG
    printf("Sent broadcast to port %d.\n", port);
#endif
  }

  return bytesSent;
}

int checkForNewPackets(int fd1, int fd2)
{
  static struct timeval tv = {0};
  fd_set rfds;
	int rc = 0;
	int ret = 0;

	// update time if necessary.
	// may want to set a flag
	// to say that stuff should be sent.
	if (tv.tv_sec == 0 && tv.tv_usec == 0)
	{
		tv.tv_sec = 0;
		tv.tv_usec = UPDATE_INTERVAL;
	}
	// Setup select
	FD_ZERO(&rfds);
	FD_SET(fd1, &rfds);
	FD_SET(fd2, &rfds);
	// Needs largest file descriptor plus 1
	rc = select( fd1>fd2 ? fd1+1 : fd2+1, &rfds, NULL, NULL, &tv);

	// check if a packet is available.
	// return the number associated w/
	// the file descriptor
	if (rc > 0)
	{
		ret = FD_ISSET(fd1, &rfds) ? 1 : FD_ISSET(fd2, &rfds) ? 2 : 0;
	}
	// return whether or not a packet is available.
	return ret;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);
int getBroadCastPacket(sockfd)
{
	char buf[256];
	char paddr[INET_ADDRSTRLEN];
	size_t len = 256;
	int flags = 0;
	struct sockaddr src_addr = {0};
	socklen_t addrlen = sizeof(src_addr);
	int ret = 0;

	flags = MSG_PEEK;
	ret = recvfrom(sockfd, buf, len, flags, &src_addr, &addrlen);
	if (ret < 0)
	{
		printf("Error peeking message!\n");
		return -1;
	}
	buf[ret] = 0;
	printf("Peeked: %s\n", buf);
	flags = 0;
	addrlen = sizeof(src_addr);
	bzero(&src_addr, sizeof(src_addr));
	ret = recvfrom(sockfd, buf, len, flags, &src_addr, &addrlen);
	if (ret < 0)
	{
		printf("Error receiving message!\n");
	}
	else
	{
		buf[ret] = '\0';
		printf("Received: %s\n", buf);
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&src_addr)->sin_addr), paddr, INET_ADDRSTRLEN);
		g_addr = ntohl((((struct sockaddr_in *)&src_addr)->sin_addr.s_addr));
		if (g_addr != g_myIp)
		{
			printf("Received %d bytes from %s\n", ret, paddr);
			buf[ret] = '\0';
			printf("%s\n", buf);
		}
	}
	return ret;
}

void getAcceptPacket(int fd)
{
  uint32_t clientlen = 0, connfd;
  struct sockaddr_in clientaddr = {0};
  struct hostent *host = NULL;
	int32_t rc = 0;
	char buff[64];
	char command;

	connfd = accept(fd, (struct sockaddr *)&clientaddr, &clientlen);
	host = gethostbyaddr( (const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	printf("Connected to %s\n", host->h_name);

	rc = read(fd, (void *)&command, sizeof(command));
	if (rc < 0)
	{
		perror("getAcceptPacket: Could not read from file descriptor.");
	}
	printf("Received Accept packet. Sending ok.");
	strcpy(buff, "ok");
	rc = write(fd,(void *)buff, strlen(buff));

	close(connfd);
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

void sendAcceptPacket(int port)
{
	int32_t fd = 0, rc = 0;
	char command = 1;
	char buff[80];
	if (g_addr != 0 && g_addr != g_myIp)
	{
		printf("Sending Accept packet.\n");
		// Open connection.
		fd = connectToServer(port, g_addr);
		if (fd < 0)
		{
			perror("setUserPos:Error setting up server.\n"); exit(1);
		}
		rc = write(fd, (void *)&command, sizeof(command));
		if (rc < 0)
		{
			perror("setUserPos:Error writing to socket.\n"); exit(1);
		}
		rc = read(fd, (void *)buff, strlen("ok"));
		if (rc < 0)
		{
			perror("setUserPos:Error reading from socket.\n"); exit(1);
		}
		buff[rc] = '\0';
		printf("Sent Accept. Client received: %s\n", buff);
		// Close socket.
		close(fd);
	}
}

int connectToServer(int port, uint32_t addr)
{
  int fd = 0;
	int rc = 0;
  struct sockaddr_in sockAddr = {0};
	char paddr[INET_ADDRSTRLEN];
	uint32_t n_addr = htonl(addr);
	uint32_t n_myIp = htonl(g_myIp);
	struct in_addr sin_addr = {0};

	sin_addr.s_addr = addr;
  // Fill out connection structure.
  sockAddr.sin_port = htons(port);
  sockAddr.sin_family = AF_INET;
  
  sockAddr.sin_addr.s_addr = htonl(addr);

	inet_ntop(AF_INET, &n_addr, paddr, INET_ADDRSTRLEN);
	printf("Connecting to %s\n", paddr);
	inet_ntop(AF_INET, &n_myIp, paddr, INET_ADDRSTRLEN);
	printf("My Ip: %s\n", paddr);

	if (g_addr != 0 && addr != g_myIp)
	{
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
	}
	return fd;
}

//uint32_t g_myIp = 0;
//uint32_t g_myMask = 0;
int wirelessConnection()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("Could not get interfaces for this device!");
		exit(EXIT_FAILURE);
	}
	
	// Loop through all interfaces.
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		// Skip interfaces w/ a null addr.
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET && strcmp(ifa->ifa_name,"wlan0") == 0 )
		{
			g_myIp = ntohl(((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr.s_addr);
			g_myMask = ntohl(((struct sockaddr_in *)(ifa->ifa_netmask))->sin_addr.s_addr);
		}
	}
	freeifaddrs(ifaddr);
}
