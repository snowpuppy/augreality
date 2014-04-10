/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * nmea.h - GPS NMEA parser for determining latitude and longitude from the Venus standard
 * GPS data
 */

#ifndef NMEA_H_
#define NMEA_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Returns the current GPS latitude.
 *
 * @return the signed GPS latitude in millionths of a degree N
 */
int32_t gpsGetLatitude();
/**
 * Returns the current GPS longitude.
 *
 * @return the signed GPS longitude in millionths of a degree E
 */
int32_t gpsGetLongitude();
/**
 * Returns the current number of satellites visible.
 *
 * @return the number of GPS satellites visible by the antenna
 */
uint32_t gpsGetSatellites();
/**
 * Parses a complete GPS line of information and updates the GPS state accordingly. The GPS
 * buffer line MUST be null-terminated!
 *
 * @param buffer the GPS data
 * @return whether GPS coordinates were successfully parsed (even if 0.000000, 0.000000!)
 */
bool gpsParse(char *buffer);

#endif
