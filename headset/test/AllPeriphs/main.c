/* Includes */

#include "main.h"
#include "nmea.h"
#include "printf.h"

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
	//spiInit();
	serialInit();
	// Enable interrupts for all peripherals
	__enable_fault_irq();
	__enable_irq();
}

/**
 * Fuel gauge test function, reports SOC and voltage every second
 */
static void testFuelGauge(void) {
	uint8_t data[4];
	unsigned int soc, v;
	while (1) {
		// Address of MAX17043 is 0x36, SOC registers and Voltage registers start at 0x02
		if (i2cReadRegister(0x36, 0x02, data, 4)) {
			v = ((unsigned int)data[0] << 4) | ((unsigned int)data[1] >> 4);
			// 2.5 mV/LSB
			v = (v * 5) >> 2;
			soc = (unsigned int)data[2];
			// Report SOC
			printf("Volt:%dmV SOC:%d%%\r\n", (int)v, (int)soc);
		}
		ledToggle();
		msleep(1000UL);
	}
}

// Buffer for testGPS()
static char gpgga[128];

// Reads a line from the GPS
static int testGPSReadLine(char *buffer, char start, int length) {
	char c; int i;
	length--;
	// Wait for start character
	do {
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
static void testGPS(void) {
	while (1) {
		testGPSReadLine(gpgga, '$', sizeof(gpgga));
		// Try to parse the GPS
		if (gpsParse(gpgga)) {
			int lat = (int)gpsGetLatitude(), lon = (int)gpsGetLongitude();
			// Got it, print it out
			printf("[%2u] %d.%06d, %d.%06d\r\n", (unsigned int)gpsGetSatellites(),
				lat / 1000000, abs(lat % 1000000), lon / 1000000, abs(lon % 1000000));
		}
		ledToggle();
	}
}

/**
 * XBee test function, echoes characters with transformation (ASCII value + 1)
 */
static void testXBee(void) {
	while (1) {
		char c = fgetc(xbee);
		// Toggle LED for status, output character + 1
		ledToggle();
		fputc(c + 1, xbee);
	}
}

// SPI data to send
static const char returnString[] = "Hello World! ";
#define RETURN_STRING_LEN ((sizeof(returnString) - 1) / sizeof(char))
static uint32_t returnIndex = 0;

void spiReceivedByte(uint8_t data) {
	uint32_t gi = returnIndex;
	ledToggle();
	// Write the byte
	spiWriteByte((uint8_t)returnString[gi++]);
	// Wrap around if lots of bytes received
	if (gi >= RETURN_STRING_LEN) gi = 0;
	returnIndex = gi;
}

/**
 * SPI test function, sends "Hello World" in a loop to the Raspberry PI on request
 */
static void testSPI(void) {
	returnIndex = 1;
	// Write 1st byte
	spiWriteByte(returnString[0]);
	spiInit();
	// Wait FOREVER
	while (1) __WFI();
}

int main(void) {
	// Sys init
	init();
	ledOff();
	testSPI();
	return 0;
}
