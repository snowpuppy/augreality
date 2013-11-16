/* Includes */

#include "main.h"
#include "imu.h"
#include "printf.h"
#include "usb.h"
#include "core_cmInstr.h"
#include "../../../ccu/src/library/packets.h"
#include "nmea.h"

// States of the headset.
// The initial state is
// BROADCASTSTATE
#define BROADCASTSTATE 0
#define LOADSTATICDATA 1
#define RUNSIMULATION  2

// Packet Types for the
// headset.
#define ACCEPTPACKET 0
#define UPDATEOBJPACKET 1
#define ENDSIMPACKET 2
#define BACKPACKET 3
#define STATICDATAPACKET 4

// Define number of bytes for
// headset updates to gpu.
#define HEADSETDATABYTES 22
#define DECIMALSPERDEGLAT 111320
#define DECIMALSPERDEGLON 78710

// Spi shared data.
// Data ready indicator.
// Buffer, length of data,
// and synchronization primitive.
/*
static int spiDataReady = 0;
static uint8_t spiBuf[256] = {0};
static uint8_t spiNumBytesRdy = 0;
static uint8_t spiMutex = 0;
*/

// Global Variables
static uint8_t headsetData[HEADSETDATABYTES];
static float originLat = 0, originLon = 0;

// Function declarations
static void serializeBroadcast(uint8_t *buffer, broadCastPacket_t *packet);
static void sendBroadcast(uint16_t netAddr, float latitude, float longitude);
static void getBytes(uint8_t *data, uint32_t numBytes);
static void processXbeeData(uint8_t *state);
void spiReceivedByte(uint8_t data);
static int GPSReadLine(char *buffer, char start, int length);
static void processGPSData(void);
static void processIMUData(void);

/**
 * System initialization
 */
static void init(void)
{
    // Tick every 1ms
    SysTick_Config(168000);
    // Set up status LED
    gpioInit();
    // Set up the peripherals
    i2cInit();
    serialInit();
    usbVCPInit();
    // write first byte of spi as zero.
    // then initialize it. This is done
    // because the gpu will interpret this
    // as the number of bytes to read.
    spiWriteByte(0);
    spiInit();
    // Initialize the IMU
    imu9Init();
    // Enable interrupts for all peripherals
    __enable_fault_irq();
    __enable_irq();
}

// Function: main
// Starting point for all other functions.
int main(void)
{
    // debug buffer.
    uint8_t data[64];
    // System variables.
    uint8_t state = BROADCASTSTATE;
    // Sys init
    init();
    ledOff();

    // main loop.
    while (1)
    {
        // Loop bytes back (for debug)
		uint32_t count = usbVCPRead(data, sizeof(data) / sizeof(uint8_t));
		if (count > 0)
			// Send the buffer back to the PC
			usbVCPWrite(data, count);

        // Process Wireless information.
        //processXbeeData(&state);
        processGPSData();
        processIMUData();

        __WFI();
    }
    return 0;
}

// Function: lock()
// This function creates a locking mechanism
// that can be used to access sensative data
// through the use of a mutex.
/*
static void lock(uint8_t *mutex)
{
    uint8_t ret = 0;
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
static void unlock(uint8_t *mutex)
{
    *mutex = 0;
}

/* Function: getBytes()
 *
 * Reads specified number of bytes into given buffer.
 *
 */
static void getBytes(uint8_t *data, uint32_t numBytes)
{
    uint32_t i = 0;
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
 * The XBEE port is checked to see if any data is available for reading.
 * If data is available for reading, then the packet type will be checked
 * and appropriate data will be immediately sent over spi. It is assumed
 * that spi is running much faster than xbee traffic.
 */
static void processXbeeData(uint8_t *state)
{
    uint8_t buf[256];
    uint8_t numBytes = 0;
    uint32_t i = 0;
    // If packets are available, then just
    // keep processing them.
    while (fcount(xbee) > 0)
    {
      // If bytes are available, then receive packet.
      // Send packet to spi or trigger appropriate action.
      getBytes(buf,3);
      if (buf[0] == 'P' && buf[1] == 'A' && buf[2] == 'C')
      {
        //fputc('a', xbee);
        // Get type of packet
        getBytes(buf,1);
        switch (buf[0])
        {
          case ACCEPTPACKET:
          case UPDATEOBJPACKET:
          case ENDSIMPACKET:
          case BACKPACKET:
          case STATICDATAPACKET:
            // Get number of bytes
            getBytes(buf,1);
            spiWriteByte(buf[0]);
            numBytes = (uint8_t)buf[0];

            // get remaining bytes from wireless.
            getBytes(buf,(uint32_t)numBytes);
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
            break;
          default:
            break;
        }
      }
    }
}

// Reads a line from the GPS
static int GPSReadLine(char *buffer, char start, int length)
{
    char c; int i;
    length--;
    // Wait for start character
    do
    {
        c = fgetc(gps);
    } while (c != start);
    // Pull up to length characters or until we hit new line
    for (i = 0; i < length && ((c = fgetc(gps)) != '\r' && c != '\n'); i++)
        *buffer++ = c;
    *buffer = 0;
    return i;
}

/**
 * GPS test function, reports new fixes to the serial port as they occur
 */
static void processGPSData(void)
{
    char gpsData[128]; float x = 0, y = 0; float latf = 0, lonf = 0;

    // Stuff the buffer early.
    //*((float *)&headsetData[8]) = 123.45;  // pitch
    //*((float *)&headsetData[12]) = 123.45; // roll
    //*((float *)&headsetData[16]) = 123.45; // yaw

    GPSReadLine(gpsData, '$', sizeof(gpsData));
    // Try to parse the GPS
    if (gpsParse(gpsData))
    {
        int lat = (int)gpsGetLatitude(), lon = (int)gpsGetLongitude();
        // Got it, print it out
        //printf("[%2u] %d.%06d, %d.%06d\r\n", (unsigned int)gpsGetSatellites(),
        //	lat / 1000000, abs(lat % 1000000), lon / 1000000, abs(lon % 1000000));
        // Stuff data into buffer for spi.

        // Find origin as first lat/lon coordinate.
        latf = (float)lat / (float)1000000;
        lonf = (float)lon / (float)1000000;
        if (originLat == 0 && originLon == 0)
        {
            originLat = latf;
            originLon = lonf;
        }
        // Find difference bet. locations.
        latf = latf - originLat;
        lonf = lonf - originLon;
        // Find offset in meters.
        x = latf*DECIMALSPERDEGLAT;
        y = lonf*DECIMALSPERDEGLON;
        // Stuff the buffer.
        *((float *)&headsetData[0]) = x; // x pos
        *((float *)&headsetData[4]) = y; // y pos
        //*((float *)&headsetData[8]) = 123.45;  // pitch
        //*((float *)&headsetData[12]) = 123.45; // roll
        //*((float *)&headsetData[16]) = 123.45; // yaw
        headsetData[20] = 23; // rssi
        headsetData[20] = 43; // FuelGauage
    }
    ledToggle();
}

// Function: processIMUData
// Purpose: Stuffs current IMU values in buffer
// so that they can be sent to the GPU and over
// wireless to the CCU.
static void processIMUData(void)
{
    ivector g, a, m;
    imu9Read(&g, &a, &m);
    //printf("Pitch:%f\tYaw:%f\tRoll:%f\r\n", a_pitch(a)*180./PI, m_pr_yaw(m, a_pitch(a), a_roll(a))*180./PI, a_roll(a)*180./PI);
    *((float *)&headsetData[8]) = a_pitch(a)*180./PI;  // pitch
    *((float *)&headsetData[12]) = a_roll(a)*180./PI; // roll
    *((float *)&headsetData[16]) = m_pr_yaw(m, a_pitch(a), a_roll(a))*180./PI; // yaw
    return;
}

// Function: spiReceivedByte
// Purpose: This function is called
// whenever a byte is received over spi.
// It is currently used to discover what
// kind of data is being requested by the gpu. The
// gpu can request xbee data or imu/gps/rssi/fuel data.
void spiReceivedByte(uint8_t data)
{
    if (data == 1)
    {
      spiWriteByte(HEADSETDATABYTES);
      spiWriteBytes(headsetData, HEADSETDATABYTES);
      spiWriteByte(0);
      //fputc(data, xbee);
    }
    // reset origin.
    if (data == 2)
    {
      originLat = 0;
      originLon = 0;
      //fputc(data,xbee);
    }
    //fputc('a',xbee);
}

// Function: serializeBroadcast
// Purpose: Convert a broadCastPacket to a byte stream
// for sending over xbee wireless.
static void serializeBroadcast(uint8_t *buffer, broadCastPacket_t *packet)
{
	buffer[0] = packet->header[0];
	buffer[1] = packet->header[1];
	buffer[2] = packet->header[2];
	buffer[3] = packet->type;
	buffer[4] = ((uint8_t *)&packet->address)[0];
	buffer[5] = ((uint8_t *)&packet->address)[1];
	buffer[6] = ((uint8_t *)&packet->latitude)[0];
	buffer[7] = ((uint8_t *)&packet->latitude)[1];
	buffer[8] = ((uint8_t *)&packet->latitude)[2];
	buffer[9] = ((uint8_t *)&packet->latitude)[3];
	buffer[10] = ((uint8_t *)&packet->longitude)[0];
	buffer[11] = ((uint8_t *)&packet->longitude)[1];
	buffer[12] = ((uint8_t *)&packet->longitude)[2];
	buffer[13] = ((uint8_t *)&packet->longitude)[3];
	buffer[14] = ((uint8_t *)&packet->crc)[0];
	buffer[15] = ((uint8_t *)&packet->crc)[1];
}

// Function: sendBroadcast
// Purpose: send a broadcast packet of the headset indicating
// that the headset is looking to be configured.
// Example:
// sendBroadcast(0x33FF, 12347.5533, 56782.5533);
static void sendBroadcast(uint16_t netAddr, float latitude, float longitude)
{
	uint8_t buffer[256];
	broadCastPacket_t packet;
	uint32_t i = 0;

	packet.header[0] = 'P';
	packet.header[1] = 'A';
	packet.header[2] = 'C';
	packet.type = 0;
	packet.address = netAddr;
	packet.latitude = 0x475dce8e; //0x2233ff44; //latitude; //0x2233ff44; //(uint32_t)(latitude*10000);
	packet.longitude = longitude; // 0x1155ee99; // (uint32_t)(longitude*10000);
	packet.crc = calcCrc((char *)&packet, sizeof(packet));
	serializeBroadcast(buffer, &packet);
	// Write to the buffer as a series of characters.
	for (i = 0; i < 16; i++)
	{
		fputc(buffer[i], xbee);
	}
	return;
}
