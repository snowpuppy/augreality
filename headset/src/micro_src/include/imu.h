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

// IMU-9 data calibrated data but not unit processed
typedef struct {
	float x, y, z;
} vector;

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
bool imu9Raw(ivector *gyro, ivector *accel, ivector *mag);
/**
 * Reads calibrated data from the 9-DOF IMU.
 *
 * @param gyro the vector to return gyro values
 * @param accel the vector to return accelerometer values
 * @param mag the vector to return magnetometer values
 * @return whether the operation completed successfully
 */
bool imu9Read(vector *gyro, vector *accel, vector *mag);
/**
 * Calibrates the IMU's gyro.
 */
void imu9Calibrate(void);

float a_pitch(vector *a);
float a_roll(vector *a);
float m_pr_yaw(vector *m, float pitch, float roll);
float g_pr_yaw(vector *g, float pitch, float roll);

#endif
