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

// XBee state constants
#define XBEEDISCOVERYSTATE 0
#define XBEEPARSINGSTATE 1

// Define number of bytes for
// headset updates to gpu.
#define HEADSETDATABYTES 22
#define NUMXBEEDATABYTES 256
#define DECIMALSPERDEGLAT 111320
#define DECIMALSPERDEGLON 78710

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
static uint8_t xbeeData[HEADSETDATABYTES];
static uint8_t numXbeeDataBytes = 0;
static uint8_t headsetData[HEADSETDATABYTES];
static float originLat = 0, originLon = 0;

// IMU processing variables
#define IMU_HISTORY 4
static uint32_t histIndex = 0;
static float gyroAccum = 0.0f;
static float pitchHist[IMU_HISTORY];
static float rollHist[IMU_HISTORY];
static float yawHist[4 * IMU_HISTORY];

// Function declarations
static void serializeBroadcast(uint8_t *buffer, broadCastPacket_t *packet);
static void sendBroadcast(uint16_t netAddr, float latitude, float longitude);
static void getBytes(uint8_t *data, uint32_t numBytes);
static void processXbeeData(uint8_t *state);
void spiReceivedByte(uint8_t data);
static uint32_t gpsReadLine(char start, uint16_t length);
static void processGPSData(void);
static void processIMUData(void);
static void processFuelGuage(void);

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
	vector g, a, m;
	float pitch, roll;
	// Take initial reading, set up gyro accumulator
	imu9Read(&g, &a, &m);
	pitch = a_pitch(&a);
	roll = a_roll(&a);
	gyroAccum = m_pr_yaw(&m, roll, pitch);
}

// Function: main
// Starting point for all other functions.
int main(void) {
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
	// main loop.
	while (1) {
		// Process Wireless information.
		//processXbeeData(&state);
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

/**
 * Function: processXbeeData
 * This function will retrieve wireless packets from the xbee uart.
 * The XBEE port is checked to see if any data is available for reading.
 * If data is available for reading, then the packet type will be checked
 * and appropriate data will be immediately sent over spi. It is assumed
 * that spi is running much faster than xbee traffic.
 */
static void processXbeeData(uint8_t *state) {
	uint8_t buf[256];
	uint32_t numBytes = 0, bytesToRead = 0;
	uint8_t bytesRead = 0;
	uint32_t i = 0;
	static xbeeState = XBEEDISCOVERYSTATE;
	// If packets are available, then just
	// keep processing them.
	while (fcount(xbee) > 4) {
		// If bytes are available, then receive packet.
		// Send packet to spi or trigger appropriate action.
		getBytes(buf, 3);
		if (buf[0] == 'P' && buf[1] == 'A' && buf[2] == 'C') {
			//fputc('a', xbee);
			// Get type of packet
			getBytes(buf, 1);
			switch (buf[0]) {
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
				xbeeState = XBEEPARSINGSTATE;
				parseStaticDataPacket();
				// Get number of bytes
				getBytes(buf, 4);
				xbeeData[0] = buf[0];
				xbeeData[1] = buf[1];
				xbeeData[2] = buf[2];
				xbeeData[3] = buf[3];
				numBytes = *((uint32_t *)buf);

				while(numBytes > 0)
				{
					// get remaining bytes from wireless.
					getBytes(buf, (uint32_t) numBytes);
					// write all bytes to spi!
					for (i = 0; i < numBytes; i++)
					{
						spiWriteByte(buf[i]);
						//fputc(buf[i], xbee);
					}
					// spiWriteByte(0);
					// Toggle LED for status, output character + 1
					ledToggle();
					fputc('b', xbee);
					//fputc(numBytes, xbee);
				}
				break;
			default:
				break;
			}
		}
	}
}

static parseStaticDataPacket(void);
{
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

// Function: imuYawUpdate
// Purpose: Perform long-term moving average filtering on the
static float imuYawUpdate(float yaw) {
	uint32_t plusMinusPi = 0; float total = 0.0f;
	yawHist[histIndex] = yaw;
	// Ring buffer
	for (uint32_t i = 0; i < 4 * IMU_HISTORY; i++) {
		float value = yawHist[i];
		if (plusMinusPi == 0) {
			// Use this to correctly handle wraps around the singularity
			if (value > 0.7f) plusMinusPi = 1;
			else if (value < -0.7f) plusMinusPi = 2;
		} else {
			// Correctly handle wraps around +/- PI
			if (value < -0.7f && plusMinusPi == 1)
				value += (float)(2.0 * PI);
			if (value > 0.7f && plusMinusPi == 2)
				value -= (float)(2.0 * PI);
		}
		total += value;
	}
	// Return averaged value
	total = total * (1.0f / 16.0f);
	if (total < (float)-PI)
		total += (float)(2.0 * PI);
	else if (total > (float)PI)
		total -= (float)(2.0 * PI);
	return total;
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
	float roll, pitch, yaw, gyroYaw, yawRate;
	// Timing related variables
	const uint32_t UPDATE_PERIOD_MS = 16;
	static unsigned long imuTimer = 0;
	unsigned long currentTime = millis();
	if (currentTime > imuTimer + UPDATE_PERIOD_MS) {
		// Index history array
		uint32_t hist = histIndex % IMU_HISTORY;
		imuTimer = currentTime;
		imu9Read(&g, &a, &m);
		// Calculate variables
		pitch = a_pitch(&a);
		roll = a_roll(&a);
		yaw = m_pr_yaw(&m, pitch, roll);
		// Get gyro rate angle in rad/s by multiplying by DT
		yawRate = g_pr_yaw(&g, pitch, roll) * (3.054326e-4f * (float)UPDATE_PERIOD_MS * 0.001f);
		// RNL and accumulate
		if (yawRate > -0.005f && yawRate < 0.005f) yawRate = 0.f;
		gyroYaw = gyroAccum + yawRate;
		// Adjust gyro yaw into range (-pi, pi)
		if (gyroYaw < (float)-PI)
			gyroYaw += (float)(2.0 * PI);
		else if (gyroYaw > (float)PI)
			gyroYaw -= (float)(2.0 * PI);
		// Compute actual yaw by rolling the gyro yaw accumulated total towards compass
		gyroYaw = gyroYaw - ((gyroYaw - yaw) * 0.008f);
		gyroAccum = gyroYaw;
		// Store in history
		pitchHist[hist] = pitch;
		rollHist[hist] = roll;
		yaw = imuYawUpdate(yaw);
		histIndex = (histIndex + 1) % (4 * IMU_HISTORY);
		// Calculate the average of the pitch, roll (4 samples)
		pitch = 0.0f;
		roll = 0.0f;
		for (uint32_t i = 0; i < IMU_HISTORY; i++) {
			pitch += pitchHist[i];
			roll += rollHist[i];
		}
		// Convert to degrees
		*((float *) &headsetData[8]) = pitch * (45.f / (float)PI); // pitch
		*((float *) &headsetData[12]) = roll * (45.f / (float)PI); // roll
		*((float *) &headsetData[16]) = yaw * (180.f / (float)PI); // yaw
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
	if (data == 1) {
		spiWriteByte(HEADSETDATABYTES);
		spiWriteBytes(headsetData, HEADSETDATABYTES);
		spiWriteByte(0);
		//fputc(data,xbee);
	}
	// reset origin.
	if (data == 2) {
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
    /*
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
    */
}

// Function: sendBroadcast
// Purpose: send a broadcast packet of the headset indicating
// that the headset is looking to be configured.
// Example:
// sendBroadcast(0x33FF, 12347.5533, 56782.5533);
static void sendBroadcast(uint16_t netAddr, float latitude, float longitude)
{
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
