#include <API.h>
#include <API_ext.h>
#include "main.h"
#include "spi.h"
#include <math.h>
#include <string.h>

char gpgga[80], gpt[32];
int gpslog[9000];

void toggleCS() {
	digitalWrite(6, HIGH);
	delay(1);
	digitalWrite(6, LOW);
}

void sendCommand(uint8_t command) {
	// Send out two blank bits
	pinMode(13, OUTPUT);
	digitalWrite(11, LOW);
	pinMode(11, OUTPUT);
	// Clock out "00"
	digitalWrite(13, LOW);
	delayMicroseconds(1);
	digitalWrite(13, HIGH);
	delayMicroseconds(1);
	digitalWrite(13, LOW);
	delayMicroseconds(1);
	digitalWrite(13, HIGH);
	delayMicroseconds(1);
	// Return to SPI control
	pinMode(13, DDR_AFO);
	pinMode(11, DDR_AFO);
	spiWrite(1, command);
}

void sendText(char *text) {
	// Send out two blank bits
	pinMode(13, OUTPUT);
	digitalWrite(11, HIGH);
	pinMode(11, OUTPUT);
	// Clock out "10"
	digitalWrite(13, LOW);
	delayMicroseconds(1);
	digitalWrite(13, HIGH);
	digitalWrite(11, LOW);
	delayMicroseconds(1);
	digitalWrite(13, LOW);
	delayMicroseconds(1);
	digitalWrite(13, HIGH);
	delayMicroseconds(1);
	// Return to SPI control
	pinMode(13, DDR_AFO);
	pinMode(11, DDR_AFO);
	spiWriteBytes(1, (unsigned char *)text, strlen(text));
}

void lcdClear() {
	// Clear the screen
	sendCommand(0x01);
	delay(1);
	sendCommand(0x02);
	delay(1);
}

int readLine(char *buffer, char start, int length) {
	char c; int i;
	length--;
	// Wait for start character
	do {
		c = fgetc(uart2);
	} while (c != start);
	// Pull up to length characters or until we hit new line
	for (i = 0; i < length && ((c = fgetc(uart2)) != '\r' && c != '\n'); i++)
		*buffer++ = c;
	*buffer = 0;
	return i;
}

int tokenize(char **string, char split, char *token) {
	char *str = *string; int count = 0;
	// Iterate until token hit or end of string
	while (*str && *str != split) {
		*token++ = *str++;
		count++;
	}
	// Skip the token and null terminate
	if (*str) str++;
	*token = 0;
	*string = str;
	return count;
}

int parse(char *buffer) {
	int value = 0; char c;
	while (*buffer) {
		c = *buffer++;
		if (c >= '0' && c <= '9')
			value = (value * 10) + (int)(c - '0');
	}
	// It's in format dd mm.mmmm so convert properly to dd.dddddd
	// Still unsigned here as we haven't parsed the "-" or "N/S/E/W"
	value = (value / 1000000) * 1000000 + (value % 1000000) * 5 / 3;
	return value;
}

int main() {
	unsigned int logPtr = 0;
	// Set up pins to boot the LCD
	pinMode(6, OUTPUT);
	digitalWrite(6, HIGH);
	// Unfortunately Maple does not support 10-bit SPI, so we need a workaround
	// Turn on SPI, but killbits the pins when required to send out dummy data
	spiInit(1, 2000000, SPI_DATA_8 | SPI_CPOL_1 | SPI_CPHA_1);
	// Wait for LCD to powerup
	delay(800);
	digitalWrite(6, LOW);
	// 8-bit mode 3x
	sendCommand(0x38);
	delay(100);
	sendCommand(0x38);
	delay(10);
	sendCommand(0x38);
	delay(10);
	// Entry mode set
	sendCommand(0x06);
	delay(2);
	// Display on no cursor
	sendCommand(0x0C);
	delay(2);
	lcdClear();
	// Loop forever
	usartInit(uart2, 38400, SERIAL_8N1);
	while (1) {
		if (fcount(stdin) > 0) {
			// Dump data on request
			char d = fgetc(stdin);
			if (d == 'd') {
				// Inefficient but good for putting in text file
				for (int i = 0; i < 9000 && gpslog[i] && gpslog[i + 1]; i += 2) {
					printf("%d,%d\n", gpslog[i], gpslog[i + 1]);
					// Otherwise the buffer gets destroyed by data deluge
					if (i % 2 == 0)
						delay(2);
				}
			}
		}
		// Read in GPS data from USART2
		readLine(gpgga, '$', 79);
		if (strncmp(gpgga, "GPGGA", 5) == 0) {
			char *str = &gpgga[6];
			int lat, lon;
			// Time
			tokenize(&str, ',', gpt);
			// Latitude
			tokenize(&str, ',', gpt);
			lat = parse(gpt);
			// N/S
			tokenize(&str, ',', gpt);
			if (gpt[0] == 'S') lat = -lat;
			// Longitude
			tokenize(&str, ',', gpt);
			lon = parse(gpt);
			// E/W
			tokenize(&str, ',', gpt);
			if (gpt[0] == 'W') lon = -lon;
			// Push lat to screen (go home)
			sendCommand(0x02);
			toggleCS();
			snprintf(gpt, 16, "%4d.%06d     ", lat / 1000000, abs(lat % 10000));
			sendText(gpt);
			// Push lon to screen (line #2)
			snprintf(gpt, 16, "%4d.%06d     ", lon / 1000000, abs(lon % 10000));
			toggleCS();
			sendCommand(0xC0);
			toggleCS();
			sendText(gpt);
			toggleCS();
			// Log the data if not zero
			if (lat != 0 && lon != 0 && (logPtr < 2 || gpslog[logPtr - 2] != lat ||
					gpslog[logPtr - 1] != lon)) {
				gpslog[logPtr++] = lat;
				gpslog[logPtr++] = lon;
			}
		}
	}
	return 0;
}
