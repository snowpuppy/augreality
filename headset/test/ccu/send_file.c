#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "../src/library/packets.h"

// Constants
#define BAUDRATE B57600
#define XBEEPORT "/dev/ttyUSB0"
#define FILENAME "output.tar"

// Other functions.
int readBytes(int fd, char *data, int numBytes);
unsigned int sizeOfFile(FILE *fp);
int readFileBytes(FILE *fp, char *data, int numBytes);
void printFloatBytes(char *buf);

// Main Function.
int main(void)
{
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[256];
  
  // File variables.
  FILE *filefp = NULL;
  unsigned int fileSize = 0;
  int bytesRead = 0;
  char message[] = "Hello There stranger. This is a test.\n";
  
  char temp;
  broadCastPacket_t packet;

  fd = open(XBEEPORT, O_RDWR | O_NOCTTY ); 
  if (fd <0) {perror(XBEEPORT); exit(-1); }

  tcgetattr(fd,&oldtio); /* save current port settings */

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);

  // Open file for reading binary
  filefp =  fopen(FILENAME, "rb");
  if (filefp == NULL)
  {
    fprintf(stderr, "Could not open file %s\n", FILENAME);
    exit(1);
  }
  fileSize = sizeOfFile(filefp);
  buf[0] = 'P';
  buf[1] = 'A';
  buf[2] = 'C';
  buf[3] = 1; // 1 indicates file transfer packet.
  buf[4] = strlen(message); // copy size of file.
  write(fd,buf,5); // send header.

  // now send payload
  write(fd,message,strlen(message));

  // now send payload

  //res = write(fd,"abcdh",5);
  
  /*
  while (!feof(filefp)) {
    // Send a file over serial...
    bytesRead = readFileBytes(filefp, buf,255);
    write(fd,buf,bytesRead);
  }
  */
  tcsetattr(fd,TCSANOW,&oldtio); // restore old port settings.
  return 0;
}

// Function "sizeOfFile"
// This function returns the total number of
// bytes in a file so that this value can be
// sent to the xbee.
unsigned int sizeOfFile(FILE *fp)
{
  unsigned int ret = 0;
  if (fp != NULL)
  {
    fseek(fp,0,SEEK_END);
    ret = ftell(fp);
    rewind(fp);
  }
  return ret;
}


// Function to read in a fixed number
// of bytes from a file. User is responsible
// for pointer size.
int readFileBytes(FILE *fp, char *data, int numBytes)
{
  int bytesRead = 0;
  if (fp != NULL)
  {
    bytesRead = fread(data, 1/*Size is in 1 Byte increments*/,numBytes,fp);
  }
  else
  {
    fprintf(stderr, "File pointer is NULL\n");
  }
  if (bytesRead != numBytes)
  {
    fprintf(stderr, "Did not read all bytes expected...\n");
  }
  return bytesRead;
}

// Function to read in a fixed number
// of bytes from the serial stream.
// User is responsible for pointer size.
int readBytes(int fd, char *data, int numBytes)
{
  int bytesRead = 0;
  int res = 0;
  //printf("Reading %d bytes.\n", numBytes);
  while (bytesRead < numBytes)
  {
    res = read(fd, data, numBytes - bytesRead);
    //printf("%d bytes read.\n", res);
    if (res < 0)
    {
      perror("Error reading serial data.\n");
    }
    else
    {
      // increment number of bytes read.
      // Keep reading till the expected number
      // of bytes is read.
      bytesRead += res;
    }
  }
}

// Function: printFloatBytes
// Purpose: Used to print the hex
// values of a floating point number
// as a sanity check that they are
// coming in correctly.
void printFloatBytes(char *buf)
{
  int i = 0;
  printf("Floating value:");
  for (i = 0; i < 4; i++)
  {
    printf(" [%d] = %X",i,buf[i]);
  }
  printf("\n");
}
