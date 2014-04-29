/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * main.c - Sensor microcontroller source code
 */

#include "main.h"
#include "imu.h"
#include "printf.h"
#include "usb.h"
#include "core_cmInstr.h"
#include "nmea.h"

// Define number of bytes for
// headset updates to gpu.
#define HEADSETDATABYTES 24

// Time constants
#define ONE_SECOND 1000
#define HALF_SECOND 500
#define FIFTY_MSECOND 50

#define LATOFFSET 0
#define LONOFFSET 4
#define PITOFFSET 8
#define ROLOFFSET 12
#define YAWOFFSET 16
#define SATOFFSET 20

// GPS state machine for stream reading
static struct {
	char line[128];
	uint16_t len;
	uint16_t state;
} gpsState;

// Global Variables
static uint8_t __attribute__ ((aligned(4))) headsetData[HEADSETDATABYTES];

// IMU processing variables
#define IMU_HISTORY 5
static uint32_t histIndex = 0;
static const float filterCoeffIncr = 2.f / (float)(IMU_HISTORY*IMU_HISTORY + IMU_HISTORY);
static float pitchHist[IMU_HISTORY];
static float rollHist[IMU_HISTORY];
static float yawHist[IMU_HISTORY];

// Function declarations
static uint32_t gpsReadLine(char start, uint16_t length);
static void processGPSData(void);
static void processIMUData(void);
static void transmitData(void);

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
	// Sys init
	init();
	msleep(1200L);
	// Initialize the IMU
	imu9Init();
	msleep(300L);
	imu9Calibrate();
	// Initialize IMU filtering
	imu9FilterInit();
	serialBufferClear();
	// main loop.
	while (1) {
		processGPSData();
		processIMUData();
		transmitData();
		__WFI();
	}
	return 0;
}

/**
* @brief Send the gps and imu data every 50 milliseconds over usb.
*/
static void transmitData(void) {
	static uint32_t count = 0;
	uint32_t bytesWritten = 0, i = 100;
	if (millis() > count + FIFTY_MSECOND) {
		ledToggle();
		count = millis();
		if (usbVCPConnected())
			// Write all bytes, even if buffer needs to evicted by the host
			do {
				bytesWritten += usbVCPWrite(headsetData + bytesWritten, HEADSETDATABYTES -
					bytesWritten);
			} while (bytesWritten < HEADSETDATABYTES && i-- > 0);
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
		latf = (float)lat * 0.000001f;
		lonf = (float)lon * 0.000001f;
		// Stuff the buffer.
		*((float *)&headsetData[LATOFFSET]) = latf; // x pos
		*((float *)&headsetData[LONOFFSET]) = lonf; // y pos
		*((float *)&headsetData[SATOFFSET]) = 0.0; //(float)gpsGetSatellites(); // num satellites as a float.
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
	ivector g, a, m;
	float roll, pitch, yaw, yawOffset, yawTmp;
	// Timing related variables
	const uint32_t UPDATE_PERIOD_MS = 16;
	static unsigned long imuTimer = 0;
	unsigned long currentTime = millis();
	if (currentTime > imuTimer + UPDATE_PERIOD_MS) {
		// Index history array
		imuTimer = currentTime;
		// push back pitch/yaw/roll history
		for (histIndex = 0; histIndex < IMU_HISTORY-1; ++histIndex) {
			pitchHist[histIndex] = pitchHist[histIndex+1];
			yawHist[histIndex] = yawHist[histIndex+1];
			rollHist[histIndex] = rollHist[histIndex+1];
		}
		// get new sensor data
		imu9Read(&g, &a, &m);
		// calculate pitch/yaw/roll based on new sensor data
		pitchHist[histIndex] = a_pitch(a);
		rollHist[histIndex] = a_roll(a);
		yawHist[histIndex] =  m_pr_yaw(m, pitchHist[histIndex], rollHist[histIndex]);

		// filter pitch/yaw/roll values
		pitch = 0;
		yaw = 0;
		roll = 0;
		// center yaw range around most recent yaw measurement
		// i.e. [yaw_recent-PI,yaw_recent+PI]
		// (assumes not more than PI differential from 
		yawOffset = yawHist[IMU_HISTORY-1];
		for (histIndex = 0; histIndex < IMU_HISTORY; ++histIndex) {
			pitch += pitchHist[histIndex] * filterCoeffIncr*(histIndex+1);
			roll += rollHist[histIndex] * filterCoeffIncr*(histIndex+1);
			yawTmp = yawHist[histIndex] - yawOffset;
			if (yawTmp > PI) {
				yawTmp -= 2*PI;
			} else if (yawTmp < -PI) {
				yawTmp += 2*PI;
			}
			yaw += yawTmp * filterCoeffIncr*(histIndex+1);
		}
		// convert yaw range back to [-PI,+PI]
		yaw += yawOffset;
		if (yawTmp > PI) {
			yawTmp -= 2*PI;
		} else if (yawTmp < -PI) {
			yawTmp += 2*PI;
		}
		// Fix the pitch and roll parameters because
		// for some reason they are wrong when they come out.
		yawTmp = roll;
		roll = -pitch;
		pitch = -yawTmp;
		yaw = -yaw;
		// Convert to degrees
		*((float *) &headsetData[PITOFFSET]) = pitch * (180. / PI); // pitch
		*((float *) &headsetData[ROLOFFSET]) = roll * (180. / PI); // roll
		*((float *) &headsetData[YAWOFFSET]) = yaw * (180. / PI); // yaw
	}
}
