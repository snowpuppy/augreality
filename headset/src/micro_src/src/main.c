/* Includes */

#include "main.h"
#include "imu.h"
#include "printf.h"
#include "usb.h"
#include "core_cmInstr.h"
#include "packets.h"
#include "nmea.h"

// States of the headset.
// The initial state is
// BROADCASTSTATE
#define BROADCASTSTATE 0
#define RUNSIMULATION  2

// XBee state constants
#define XBEEDISCOVERYSTATE 0
#define XBEEPARSINGSTATE 1

// Define number of bytes for
// headset updates to gpu.
#define HEADSETDATABYTES 22
#define NUMXBEEDATABYTES 256
#define DECIMALSPERDEGLAT 111320
#define DECIMALSPERDEGLON 78710

// SPI COMMANDS (from gpu to headset)
#define GETHEADSETDATA 1
#define FLUSHSPIBUFFER 2
#define GETXBEEDATA 3

// IMU constants
#define HIST_SIZE 5

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

// GPS state machine for stream reading
static struct {
	char line[128];
	uint16_t len;
	uint16_t state;
} gpsState;

// Global Variables
static uint8_t xbeeData[COMM_BUFFER_SIZE];
static uint8_t numXbeeDataBytes = 0;
static uint8_t headsetData[HEADSETDATABYTES];
static float originLat = 0, originLon = 0;

// IMU processing variables
#define IMU_HISTORY 5
static uint32_t histIndex = 0;
const float filterCoeffs[IMU_HISTORY] = {.10, .15, .20, .25, .30};
static float pitchHist[IMU_HISTORY];
static float rollHist[IMU_HISTORY];
static float yawHist[IMU_HISTORY];

// Function declarations
static void sendBroadcast(uint16_t netAddr, float latitude, float longitude);
static void getBytes(uint8_t *data, uint32_t numBytes);
static void processXbeeData();
void spiReceivedByte(uint8_t data);
static uint32_t gpsReadLine(char start, uint16_t length);
static void processGPSData(void);
static void processIMUData(void);
static void processFuelGuage(void);
static void parseStaticDataPacket(uint32_t *state);
void xbeeInit(uint8_t *xbeeId);

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
	usbVCPInit();
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

/**
 * IMU filtering initialization
 */
static void imu9FilterInit(void) {
	// Zero history vectors
	for (histIndex = 0; histIndex < IMU_HISTORY; ++histIndex) {
		pitchHist[histIndex] = 0;
		rollHist[histIndex] = 0;
		yawHist[histIndex] = 0;
	}
}

// Function: main
// Starting point for all other functions.
int main(void) {
	uint8_t xbeeId[16] = {0};
	// Sys init
	init();
	msleep(1500L);
	// Initialize the IMU
	imu9Init();
	msleep(500L);
	imu9Calibrate();
	// Initialize IMU filtering
	imu9FilterInit();
	serialBufferClear();
	// Initialize XBee settings
	xbeeInit(xbeeId);
	// main loop.
	while (1) {
		// Process Wireless information.
		processXbeeData();
		processGPSData();
		processIMUData();
		processFuelGuage();

		__WFI();
	}
	return 0;
}

#if 0
// Function: lock()
// This function creates a locking mechanism
// that can be used to access sensative data
// through the use of a mutex.
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

// Function unlock()
// This function allows one to unlock
// a mutex. Note that this could be done by
// anybody... (but they shouldn't)
static void unlock(uint8_t *mutex)
{
    *mutex = 0;
}
#endif

/* Function: getBytes()
 *
 * Reads specified number of bytes into given buffer.
 *
 */
static void getBytes(uint8_t *data, uint32_t numBytes) {
	uint32_t i = 0;
	if (numBytes > 0) {
		for (i = 0; i < numBytes; i++) {
			data[i] = fgetc(xbee);
		}
	}
}

// Function: xbeeInit()
// Purpose: Grab the mac address
// so that it can be used to identify
// this headset.
void xbeeInit(uint8_t *xbeeId)
{
	fputc('+',xbee);
	fputc('+',xbee);
	fputc('+',xbee);
	// Wait for ok.
	getBytes(xbeeId,2);
	if (xbeeId[0] == 'O' && xbeeId[1] == 'K')
	{
		fputc('A',xbee);
		fputc('T',xbee);
		fputc('S',xbee);
		fputc('H',xbee);
		fputc('\r',xbee);
		getBytes(&xbeeId[8],8);
		fputc('A',xbee);
		fputc('T',xbee);
		fputc('S',xbee);
		fputc('L',xbee);
		fputc('\r',xbee);
		getBytes(xbeeId,8);
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
static void processXbeeData() {
	// use a small buffer to detect the type
	// of packet.
	static uint8_t buf[16];
	static uint32_t xbeeState = XBEEDISCOVERYSTATE;
	// If packets are available
	// then start processing them.
	if (fcount(xbee) > 2 && xbeeState == XBEEDISCOVERYSTATE) {
		// If bytes are available, then receive packet.
		// Send packet to spi or trigger appropriate action.
		// This is a sequence detector for 'P','A','C'
		getBytes(buf, 1);
		if (buf[2] == 'P' && buf[1] == 'A' && buf[0] == 'C') {
			// Get type of packet
			getBytes(buf, 1);
			switch (buf[0]) {
			case ACCEPTHEADSET:
				xbeeState = ACCEPTHEADSET;
				break;
			case UPDATEOBJINSTANCE:
				xbeeState = UPDATEOBJINSTANCE;
				break;
			case ENDSIMULATION:
				xbeeState = ENDSIMULATION;
				break;
			case STARTSIMULATION:
				xbeeState = STARTSIMULATION;
				break;
			case GOBACK:
				xbeeState = GOBACK;
				break;
			case LOADSTATICDATA:
				xbeeState = LOADSTATICDATA;
				break;
			default:
				break;
			}
		}
		else {
			buf[2] = buf[1];
			buf[1] = buf[0];
		}
	}
	// Process packets depending on the
	// packet we're expecting.
	switch (xbeeState) {
		case ACCEPTHEADSET:
			break;
		case UPDATEOBJINSTANCE:
			break;
		case ENDSIMULATION:
			break;
		case STARTSIMULATION:
			break;
		case GOBACK:
			break;
		case LOADSTATICDATA:
			parseStaticDataPacket(&xbeeState);
			break;
		default:
			break;
	}
}

static void parseStaticDataPacket(uint32_t *state) {
	static uint32_t numBytesLeft = 0;
	// indicate number of bytes that should be read:
	// either 64 or numBytesLeft
	uint32_t bytesToRead = 0;
	static uint32_t i_local = 0;
	static uint8_t foundNumBytes = 0;

	// Extract the number of bytes for the
	// file in advance.
	if (!foundNumBytes && fcount(xbee) > 4) {
		// Get number of bytes to read
		xbeeData[0] = LOADSTATICDATA;
		getBytes(&xbeeData[1], 4);
		numBytesLeft = *((uint32_t *)&xbeeData[1]);
		i_local = 5;
		foundNumBytes = 1;
		//fputc('a', xbee);
	}

	// calculate number of bytes to read
	bytesToRead = numBytesLeft < (COMM_BUFFER_SIZE/16) ? numBytesLeft : (COMM_BUFFER_SIZE/16);

	if (foundNumBytes && (fcount(xbee) >= bytesToRead) && numXbeeDataBytes == 0) {
		// get remaining bytes from wireless.
		__disable_irq();
		getBytes(&xbeeData[i_local], (uint32_t)bytesToRead);
		numBytesLeft -= bytesToRead;
		// Protect updating the number of bytes to read.
		numXbeeDataBytes = bytesToRead + i_local;
		__enable_irq();

		// If we're done, reset to get next packet.
		if (numBytesLeft <= 0) {
			*state = XBEEDISCOVERYSTATE; 
			foundNumBytes = 0;
			numBytesLeft = 0;
		}
		i_local = 0;
		// Toggle LED for status, output character + 1
		//ledToggle();
		//fputc('b', xbee);
	}
}

// Reads a line from the GPS
static uint32_t gpsReadLine(char start, uint16_t length) {
	char c;
	while (fcount(gps) > 0) {
		c = fgetc(gps);
		if (gpsState.state == 0) {
			// Wait for start
			if (c == start) {
				gpsState.state = 1;
				gpsState.len = 0;
			}
		} else {
			// Increment length
			uint16_t len = gpsState.len;
			if (len >= (length - 1) || c == '\r' || c == '\n') {
				// END OF LINE!
				gpsState.line[len] = 0;
				gpsState.state = 0;
				return 1;
			} else
				gpsState.line[len] = c;
			gpsState.len = len + 1;
		}
	}
	return 0;
}

/**
 * GPS test function, reports new fixes to the serial port as they occur
 */
static void processGPSData(void) {
	float latf = 0, lonf = 0;
	// Try to parse the GPS
	if (gpsReadLine('$', sizeof(gpsState.line)) && gpsParse(gpsState.line)) {
		int lat = (int)gpsGetLatitude(), lon = (int)gpsGetLongitude();
		// Find origin as first lat/lon coordinate.
		latf = (float)lat * 0.000001f;
		lonf = (float)lon * 0.000001f;
		if (originLat == 0.f && originLon == 0.f && gpsGetSatellites() > 2) {
			originLat = latf;
			originLon = lonf;
		}
		// Find difference bet. locations.
		latf = latf - originLat;
		lonf = lonf - originLon;
		// Stuff the buffer.
		*((float *)&headsetData[0]) = latf * DECIMALSPERDEGLAT; // x pos
		*((float *)&headsetData[4]) = lonf * DECIMALSPERDEGLON; // y pos
	}
}

// Function: processIMUData
// Purpose: Stuffs current IMU values in buffer
// so that they can be sent to the GPU and over
// wireless to the CCU. This occurs every 16
// milliseconds.
// Rssi is also set to be sent through spi.
static void processIMUData(void) {
	// Processing variables
	vector g, a, m;
	float roll, pitch, yaw;
	// Timing related variables
	const uint32_t UPDATE_PERIOD_MS = 16;
	static unsigned long imuTimer = 0;
	unsigned long currentTime = millis();
	if (currentTime > imuTimer + UPDATE_PERIOD_MS) {
		// Index history array
		uint32_t hist = histIndex % IMU_HISTORY;
		imuTimer = currentTime;
		// push back pitch/yaw/roll history
		for (histIndex = 0; histIndex < IMU_HISTORY-1; ++histIndex) {
			pitchHist[histIndex] = pitchHist[histIndex+1];
			yawHist[histIndex] = yawHist[histIndex+1];
			rollHist[histIndex] = rollHist[histIndex+1];
		}
		// get new sensor data
		imu9Raw(&g, &a, &m);
		// calculate pitch/yaw/roll based on new sensor data
		pitchHist[histIndex] = a_pitch(a);
		rollHist[histIndex] = a_roll(a);
		yawHist[histIndex] =
				m_pr_yaw(m, rollHist[histIndex], pitchHist[histIndex]);
				// roll and pitch swapped due to physical orientation of sensor

		// filter pitch/yaw/roll values
		pitch = 0;
		yaw = 0;
		roll = 0;
		for (histIndex = 0; histIndex < HIST_SIZE; ++histIndex) {
			pitch += pitchHist[histIndex] * filterCoeffs[histIndex];
			yaw += yawHist[histIndex] * filterCoeffs[histIndex];
			roll += rollHist[histIndex] * filterCoeffs[histIndex];
		}
		// Convert to degrees
		*((float *) &headsetData[8]) = pitch * (180. / PI); // pitch
		*((float *) &headsetData[12]) = roll * (180. / PI); // roll
		*((float *) &headsetData[16]) = yaw * (180. / PI); // yaw
		headsetData[20] = (char) gpioGetRSSI(); // rssi
		ledToggle();
	}
}

// Function: processFuelGuage
// This function waits 1 second then sends for the
// battery percentage from the fuel guage over I2C.
// This value is put into the buffer that the 
// spiReceivedByte() function sends over spi.
static void processFuelGuage(void) {
	uint8_t data[4];
	unsigned int soc, v;
	static unsigned long fuelTimer = 0;
	unsigned long currentTime = 0;
	currentTime = millis();
	// Check timer value for every second, then
	// request data for battery charge.
	if (currentTime > fuelTimer + 1000) {
		fuelTimer = currentTime;
		// Address of MAX17043 is 0x36, SOC registers and Voltage registers start at 0x02
		if (i2cReadRegister(0x36, 0x02, data, 4)) {
			v = ((unsigned int) data[0] << 4) | ((unsigned int) data[1] >> 4);
			// 2.5 mV/LSB
			v = (v * 5) >> 2;
			soc = (unsigned int) data[2];
			// Queue the fuel guage for sending over spi.
			headsetData[21] = (char) soc; // FuelGauage
		}
	}
}

// Function: spiReceivedByte
// Purpose: This function is called
// whenever a byte is received over spi.
// It is currently used to discover what
// kind of data is being requested by the gpu. The
// gpu can request xbee data or imu/gps/rssi/fuel data.
void spiReceivedByte(uint8_t data) {
	if (data == GETHEADSETDATA) {
		spiWriteByte(HEADSETDATABYTES);
		spiWriteBytes(headsetData, HEADSETDATABYTES);
		spiWriteByte(0);
		//fputc(data,xbee);
	}
	// reset origin.
	if (data == FLUSHSPIBUFFER) {
		// Reset origin for GPS. This may need to
		// be changed to a seperate command.
		// (I would like to eliminate it by sampling
		//  the GPS signal till it become stable.)
		originLat = 0, originLon = 0;
		// Empty the buffer and push a zero.
		emptySpiBuffer();
		spiWriteByte(0);
		//fputc('a',xbee);
	}
	// Send XBee packet data
	if (data == GETXBEEDATA) {
		if (numXbeeDataBytes > 0) {
			printf("%d\n", numXbeeDataBytes);
			spiWriteByte(numXbeeDataBytes);
			spiWriteBytes(xbeeData, numXbeeDataBytes);
			spiWriteByte(0);
			numXbeeDataBytes = 0;
		}
		else
		{
			// Confirm that a request was sent,
			// but no data available.
			spiWriteByte(1);
			spiWriteByte(0);
		}
	}
}

// Function: sendBroadcast
// Purpose: send a broadcast packet of the headset indicating
// that the headset is looking to be configured.
// Example:
// sendBroadcast(0x33FF, 12347.5533, 56782.5533);
static void sendBroadcast(uint16_t netAddr, float latitude, float longitude) {
    /*
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
    */
	return;
}
