/*
 * imu.h - 9-DOF IMU access and hardware support functions for reading gyro, accelerometer, and
 * compass data from the onboard L3DG20 and LSM303DHLC chips
 */

#ifndef IMU_H_
#define IMU_H_

#include <stdbool.h>
#include <stdint.h>

// Vector of integer values corresponding to raw data from the 9-DOF IMU
// NOT processed into units of reasonable significance
typedef struct {
	int16_t ix, iy, iz;
} ivector;

/**
 * Brings up the 9-DOF IMU in the default configuration.
 */
void imu9Init();
/**
 * Reads raw data from the 9-DOF IMU.
 *
 * @param gyro the vector to return gyro values
 * @param accel the vector to return accelerometer values
 * @param mag the vector to return magnetometer values
 * @return whether the operation completed successfully
 */
bool imu9Read(ivector *gyro, ivector *accel, ivector *mag);

#endif
