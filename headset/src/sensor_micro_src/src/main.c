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

// Define number of bytes for headset updates to gpu.
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
#define filterCoeffIncr (2.f / (float)(IMU_HISTORY * IMU_HISTORY + IMU_HISTORY))
static uint32_t histIndex = 0;
#ifdef USE_COMPLEMENTARY_FILTER
static float gyroAccum = 0.0f;
#endif
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
	for (uint32_t i = 0; i < IMU_HISTORY; i++) {
		pitchHist[i] = 0;
		rollHist[i] = 0;
		yawHist[i] = 0;
	}
}

// Function: main
// Starting point for all other functions.
int main(void) {
	// Sys init
	init();
	msleep(200L);
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
		*((float *)&headsetData[SATOFFSET]) = (float)gpsGetSatellites(); // num satellites as a float.
	}
}

#ifdef USE_COMPLEMENTARY_FILTER
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
#endif

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
	if (currentTime >= imuTimer + UPDATE_PERIOD_MS) {
		// Index history array
		uint32_t hist = histIndex;
		imuTimer = currentTime;
		imu9Read(&g, &a, &m);
		// Calculate variables
		pitch = a_pitch(&a);
		roll = a_roll(&a);
		yaw = m_pr_yaw(&m, pitch, roll, 0.0f);
#ifdef USE_COMPLEMENTARY_FILTER
		float gyroYaw, yawRate;
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
		histIndex = (hist + 1) % IMU_HISTORY;
		// Calculate the average of the pitch, roll (4 samples)
		pitch = 0.0f;
		roll = 0.0f;
		for (uint32_t i = 0; i < IMU_HISTORY; i++) {
			pitch += pitchHist[i];
			roll += rollHist[i];
		}
		pitch *= 0.25f;
		roll *= 0.25f;
#else
		float yawOffset, yawTmp;
		// push back pitch/yaw/roll history
		for (uint32_t i = 0; i < IMU_HISTORY - 1; i++) {
			pitchHist[i] = pitchHist[i + 1];
			yawHist[i] = yawHist[i + 1];
			rollHist[i] = rollHist[i + 1];
		}
		// center yaw range around most recent yaw measurement
		// i.e. [yaw_recent-PI,yaw_recent+PI]
		// (assumes not more than PI differential from initial)
		yawOffset = yawHist[IMU_HISTORY - 1];
		for (uint32_t i = 0; i < IMU_HISTORY; i++) {
			pitch += pitchHist[i] * filterCoeffIncr * (float)(i + 1);
			roll += rollHist[i] * filterCoeffIncr * (float)(i + 1);
			yawTmp = yawHist[i] - yawOffset;
			if (yawTmp > PI) {
				yawTmp -= 2.f*PI;
			} else if (yawTmp < -PI) {
				yawTmp += 2.f*PI;
			}
			yaw += yawTmp * filterCoeffIncr * (float)(i + 1);
		}
		// convert yaw range back to [-PI,+PI]
		yaw += yawOffset;
		if (yawTmp > PI) {
			yawTmp -= 2*PI;
		} else if (yawTmp < -PI) {
			yawTmp += 2*PI;
		}
		// Store in history
		pitchHist[hist] = pitch;
		rollHist[hist] = roll;
		yawHist[hist] = yaw;
#endif
		const float oldRoll = roll;
		roll = -pitch;
		yaw = -yaw;
		pitch = -oldRoll;
		// Convert to degrees
#ifndef HARD_IRON_CAL
		*((float *) &headsetData[PITOFFSET]) = pitch * (180.f / (float)PI); // pitch
		*((float *) &headsetData[ROLOFFSET]) = roll * (180.f / (float)PI); // roll
		*((float *) &headsetData[YAWOFFSET]) = yaw * (180.f / (float)PI); // yaw
#else
		*((float *) &headsetData[PITOFFSET]) = m.x;
		*((float *) &headsetData[ROLOFFSET]) = m.y;
		*((float *) &headsetData[YAWOFFSET]) = m.z;
#endif
	}
}
