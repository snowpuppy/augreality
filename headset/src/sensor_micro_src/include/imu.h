/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
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
bool imu9Read(vector *gyro, vector *accel, vector *mag);
void imu9Calibrate(void);

// Function: a_pitch
// Purpose: This function returns accelerometer pitch value?
float a_pitch(vector *a);
// Function: g_pr_yaw
// Purpose: Calculate gyro yaw rate given IMU data.
float g_pr_yaw(vector *g, float pitch, float roll);
// Function: a_roll
// Purpose: Calculate roll information.
float a_roll(vector *a);
// Function: a_roll
// Purpose: Calculate roll information.
float m_pr_yaw(vector *m, float pitch, float roll, float yaw);

#endif
