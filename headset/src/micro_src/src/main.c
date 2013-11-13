/* Includes */

#include "main.h"
#include "imu.h"
#include "printf.h"
#include "usb.h"
#include "core_cmInstr.h"
#include "packets.h"
#include "int_sizes.h"


// Spi shared data.
// Data ready indicator.
// Buffer, length of data,
// and synchronization primitive.
/*
static int spiDataReady = 0;
static char spiBuf[256] = {0};
static char spiNumBytesRdy = 0;
static char spiMutex = 0;
*/

// Function declarations
static void serializeBroadcast(char *buffer, broadCastPacket_t *packet);
static void sendBroadcast(int16 netAddr, float latitude, float longitude);
static void getBytes(char *data, int numBytes);
static void processXbeeData(void);
void spiReceivedByte(uint8_t data);

/**
 * USB initialization
 */
static void usbInit(void) {
    VCP_Init();
    VCP_SetUSBTxBlocking(0);
}

/**
 * System initialization
 */
static void init(void) {
    // Tick every 1ms
    SysTick_Config(168000);
    // Set up status LED
    gpioInit();
    // Set up the peripherals
    i2cInit();
    serialInit();
    usbInit();
    // write first byte of spi as zero.
    // then initialize it. This is done
    // because the gpu will interpret this
    // as the number of bytes to read.
    spiWriteByte(0);
    spiInit();
    // Enable interrupts for all peripherals
    __enable_fault_irq();
    __enable_irq();
}

// Function: main
// Starting point for all other functions.
int main(void) {
    uint8_t data[64];
    // Sys init
    init();
    ledOff();
    // initialize spi.
    //spiInit();
    // main loop.
    while (1) {
        // Loop bytes back
        uint32_t count = VCP_BytesAvailable();
        if (count > 0) {
            // Pull up to 64 bytes
            if (count > 64) count = 64;
            for (uint32_t i = 0; i < count; i++)
                data[i] = VCP_GetByte();
            // Send them back
            VCP_DataTx(data, count);
        }

        // Process Wireless information.
        processXbeeData();

        __WFI();
    }
    return 0;
}

// Function: lock()
// This function creates a locking mechanism
// that can be used to access sensative data
// through the use of a mutex.
/*
static void lock(char *mutex)
{
    char ret = 0;
    // Attempt to claim the mutex.
    do
    {
        // Keep trying while the mutex is
        // being held.
        do
        {
            // the mutex value is 1 if in use.
            // the mutex value is 0 if available.
            ret = __ldrex(mutex);
        } while (ret == 1);
        // strex returns 0 on success.
        // strex returns 1 on failure.
        ret = __strex(1, mutex);
    } while (ret == 1);
}
*/

// Function unlock()
// This function allows one to unlock
// a mutex. Note that this could be done by
// anybody... (but they shouldn't)
static void unlock(char *mutex)
{
    *mutex = 0;
}

/* Function: getBytes()
 *
 * Reads specified number of bytes into given buffer.
 *
 */
static void getBytes(char *data, int numBytes)
{
    int i = 0;
    if (numBytes > 0)
    {
        for (i = 0; i < numBytes; i++)
        {
            data[i] = fgetc(xbee);
        }
    }
}

/**
 * Function: processXbeeData
 * This function will retrieve wireless packets from the xbee uart.
 * It had better not be a blocking call to read the data.
 */
static void processXbeeData(void)
{
    char buf[256];
    unsigned char numBytes = 0;
    int i = 0;
    getBytes(buf,3);
    if (buf[0] == 'P' && buf[1] == 'A' && buf[2] == 'C')
    {
        //fputc('a', xbee);
        /*
        getBytes(buf,1);
        lock(spiMutex);
        spiDataReady = 1;
        unlock(spiMutex);
        */
        //while (spiDataReady == 1);
        getBytes(buf,1);
        getBytes(buf,1);
        spiWriteByte(buf[0]);
        numBytes = (unsigned char)buf[0];

        // get remaining bytes from wireless.
        getBytes(buf,(int)numBytes);
        // write all bytes to spi!
        for (i = 0; i < numBytes; i++)
        {
            spiWriteByte(buf[i]);
            //fputc(buf[i], xbee);
        }
        spiWriteByte(0);
        // Toggle LED for status, output character + 1
        ledToggle();
        fputc('b', xbee);
        //fputc(numBytes, xbee);
    }
}

/**
 * SPI test function, sends "Hello World" in a loop to the Raspberry PI on request
 */
/*
static void testSPI(void) {
    returnIndex = 1;
    // Write 1st byte
    spiWriteByte(returnString[0]);
    spiInit();
    // Wait FOREVER
    while (1) __WFI();
}*/

void spiReceivedByte(uint8_t data)
{
    //fputc('a', xbee);
    /*
    char bytesAvailable = 0;
    char numBytesRdy = 0;
    int i = 0;

    // check to see if bytes are available to read.
    lock(spiMutex);
    bytesAvailable = spiDataReady;
    numBytesRdy = spiNumBytesRdy;
    unlock(spiMutex);
    spiWriteByte(numBytesRdy);
    if (numBytesRdy > 0)
    {
        lock(spiMutex);
        for (i = 0; i < numBytesRdy; i++)
        {
            spiWriteByte(i);
        }
        unlock(spiMutex);
    }
    */
}

// Function: serializeBroadcast
// Purpose: Convert a broadCastPacket to a byte stream
// for sending over xbee wireless.
static void serializeBroadcast(char *buffer, broadCastPacket_t *packet)
{
	buffer[0] = packet->header[0];
	buffer[1] = packet->header[1];
	buffer[2] = packet->header[2];
	buffer[3] = packet->type;
	buffer[4] = ((char *)&packet->address)[0];
	buffer[5] = ((char *)&packet->address)[1];
	buffer[6] = ((char *)&packet->latitude)[0];
	buffer[7] = ((char *)&packet->latitude)[1];
	buffer[8] = ((char *)&packet->latitude)[2];
	buffer[9] = ((char *)&packet->latitude)[3];
	buffer[10] = ((char *)&packet->longitude)[0];
	buffer[11] = ((char *)&packet->longitude)[1];
	buffer[12] = ((char *)&packet->longitude)[2];
	buffer[13] = ((char *)&packet->longitude)[3];
	buffer[14] = ((char *)&packet->crc)[0];
	buffer[15] = ((char *)&packet->crc)[1];
}

// Function: sendBroadcast
// Purpose: send a broadcast packet of the headset indicating
// that the headset is looking to be configured.
// Example:
// sendBroadcast(0x33FF, 12347.5533, 56782.5533);
static void sendBroadcast(int16 netAddr, float latitude, float longitude)
{
	char buffer[256];
	broadCastPacket_t packet;
	int i = 0;

	packet.header[0] = 'P';
	packet.header[1] = 'A';
	packet.header[2] = 'C';
	packet.type = 0;
	packet.address = netAddr;
	packet.latitude = 0x475dce8e; //0x2233ff44; //latitude; //0x2233ff44; //(int32)(latitude*10000);
	packet.longitude = longitude; // 0x1155ee99; // (int32)(longitude*10000);
	packet.crc = calcCrc((char *)&packet, sizeof(packet));
	serializeBroadcast(buffer, &packet);
	// Write to the buffer as a series of characters.
	for (i = 0; i < 16; i++)
	{
		fputc(buffer[i], xbee);
	}
	return;
}
