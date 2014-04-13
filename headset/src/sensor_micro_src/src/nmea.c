/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * nmea.c - GPS NMEA parser for determining latitude and longitude from the Venus standard
 * GPS data
 */

#include "main.h"
#include "nmea.h"

// GPS latitude and longitude
static struct {
	int32_t lat;
	int32_t lon;
	uint32_t satellites;
} gpsData;

/**
 * Tokenizes a string, like strtok(), but re-entrantly.
 *
 * @param string a pointer to the start of the string
 * @param split the character used to split
 * @param token the buffer where the token will be stored
 * @param length the length of the buffer
 * @return the length of the string parsed
 */
static uint32_t tokenize(char **string, char split, char *token, uint32_t length) {
	char *str = *string; uint32_t count = 0;
	// Iterate until token hit or end of string
	while (*str && *str != split && count < length) {
		*token++ = *str++;
		count++;
	}
	// Skip the token and null terminate
	if (*str) str++;
	*token = '\0';
	*string = str;
	return count;
}

/**
 * Parses a number from the null-terminated buffer into GPS decimal degrees. Performs
 * conversion from degrees and minutes into degrees * 1E6.
 *
 * @param buffer the buffer of numeric data
 * @return the number parsed, or 0 if no number could be parsed
 */
static int32_t parse(char *buffer) {
	int32_t value = 0; char c;
	while (*buffer) {
		c = *buffer++;
		if (c >= '0' && c <= '9')
			value = (value * 10) + (int32_t)(c - '0');
	}
	// It's in format dd mm.mmmm so convert properly to dd.dddddd
	// Still unsigned here as we haven't parsed the "-" or "N/S/E/W"
	value = (value / 1000000) * 1000000 + (value % 1000000) * 5 / 3;
	return value;
}

/**
 * Returns the current GPS latitude.
 *
 * @return the signed GPS latitude in millionths of a degree N
 */
int32_t gpsGetLatitude() {
	return gpsData.lat;
}

/**
 * Returns the current GPS longitude.
 *
 * @return the signed GPS longitude in millionths of a degree E
 */
int32_t gpsGetLongitude() {
	return gpsData.lon;
}

/**
 * Returns the current number of satellites visible.
 *
 * @return the number of GPS satellites visible by the antenna
 */
uint32_t gpsGetSatellites() {
	return gpsData.satellites;
}

/**
 * Parses a complete GPS line of information and updates the GPS state accordingly. The GPS
 * buffer line MUST be null-terminated!
 *
 * @param buffer the GPS data
 * @return whether GPS coordinates were successfully parsed (even if 0.000000, 0.000000!)
 */
bool gpsParse(char *buffer) {
	int32_t lat, lon;
	if (strncmp(buffer, "GPGGA", 5) == 0) {
		char gpt[32];
		char *str = &buffer[6];
		// Time
		tokenize(&str, ',', gpt, sizeof(gpt));
		// Latitude
		tokenize(&str, ',', gpt, sizeof(gpt));
		lat = parse(gpt);
		// N/S
		tokenize(&str, ',', gpt, sizeof(gpt));
		if (gpt[0] == 'S') lat = -lat;
		gpsData.lat = lat;
		// Longitude
		tokenize(&str, ',', gpt, sizeof(gpt));
		lon = parse(gpt);
		// E/W
		tokenize(&str, ',', gpt, sizeof(gpt));
		if (gpt[0] == 'W') lon = -lon;
		gpsData.lon = lon;
		// Fix?
		tokenize(&str, ',', gpt, sizeof(gpt));
		if (gpt[0] == '1') {
			// GPS satellites in view
			tokenize(&str, ',', gpt, sizeof(gpt));
			if (gpt[0] && gpt[1]) {
				gpsData.satellites = (uint32_t)(uint8_t)(gpt[0] - '0') * 10U +
					(uint32_t)(uint8_t)(gpt[1] - '0');
				return true;
			}
		}
	}
	return false;
}
