/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * imu.c - 9-DOF IMU access and hardware support functions for reading gyro, accelerometer, and
 * compass data from the onboard L3DG20 and LSM303DHLC chips
 */

#include "arm_math.h"
#include "imu.h"
#include "main.h"

#define YELLOW_SUBMARINE

// ----- CALIBRATION -----

#ifdef YELLOW_SUBMARINE
#define M_X_MIN -380.f
#define M_Y_MIN -467.f
#define M_Z_MIN -410.f
#define M_X_MAX 403.f
#define M_Y_MAX 320.f
#define M_Z_MAX 411.f
#else
#define M_X_MIN -404.f
#define M_Y_MIN -404.f
#define M_Z_MIN -463.f
#define M_X_MAX 408.f
#define M_Y_MAX 363.f
#define M_Z_MAX 335.f
#endif

// ----- END CALIBRATION -----

// Addresses for the parts on the I2C bus
#define MAG_ADDRESS (0x3C >> 1)
#define ACC_ADDRESS (0x32 >> 1)
#define L3GD20_ADDRESS (0xD6 >> 1)

// LSM303 registers
#define LSM303_CTRL_REG1_A 0x20
#define LSM303_CTRL_REG2_A 0x21
#define LSM303_CTRL_REG3_A 0x22
#define LSM303_CTRL_REG4_A 0x23
#define LSM303_CTRL_REG5_A 0x24
#define LSM303_CTRL_REG6_A 0x25
#define LSM303_REFERENCE_A 0x26
#define LSM303_STATUS_REG_A 0x27

#define LSM303_OUT_X_L_A 0x28
#define LSM303_OUT_X_H_A 0x29
#define LSM303_OUT_Y_L_A 0x2A
#define LSM303_OUT_Y_H_A 0x2B
#define LSM303_OUT_Z_L_A 0x2C
#define LSM303_OUT_Z_H_A 0x2D

#define LSM303_FIFO_CTRL_REG_A 0x2E
#define LSM303_FIFO_SRC_REG_A 0x2F
#define LSM303_INT1_CFG_A 0x30
#define LSM303_INT1_SRC_A 0x31
#define LSM303_INT1_THS_A 0x32
#define LSM303_INT1_DURATION_A 0x33
#define LSM303_INT2_CFG_A 0x34
#define LSM303_INT2_SRC_A 0x35
#define LSM303_INT2_THS_A 0x36
#define LSM303_INT2_DURATION_A 0x37

#define LSM303_CLICK_CFG_A 0x38
#define LSM303_CLICK_SRC_A 0x39
#define LSM303_CLICK_THS_A 0x3A
#define LSM303_TIME_LIMIT_A 0x3B
#define LSM303_TIME_LATENCY_A 0x3C
#define LSM303_TIME_WINDOW_A 0x3D
#define LSM303_CRA_REG_M 0x00
#define LSM303_CRB_REG_M 0x01
#define LSM303_MR_REG_M 0x02

#define LSM303_OUT_X_H_M 0x03
#define LSM303_OUT_X_L_M 0x04
#define LSM303_OUT_Y_H_M 0x07
#define LSM303_OUT_Y_L_M 0x08
#define LSM303_OUT_Z_H_M 0x05
#define LSM303_OUT_Z_L_M 0x06
#define LSM303_SR_REG_M 0x09
#define LSM303_IRA_REG_M 0x0A
#define LSM303_IRB_REG_M 0x0B
#define LSM303_IRC_REG_M 0x0C

#define LSM303_TEMP_OUT_H_M 0x31
#define LSM303_TEMP_OUT_L_M 0x32

// L3G registers
#define L3G_WHO_AM_I 0x0F

#define L3G_CTRL_REG1 0x20
#define L3G_CTRL_REG2 0x21
#define L3G_CTRL_REG3 0x22
#define L3G_CTRL_REG4 0x23
#define L3G_CTRL_REG5 0x24
#define L3G_REFERENCE 0x25
#define L3G_OUT_TEMP 0x26
#define L3G_STATUS_REG 0x27

#define L3G_OUT_X_L 0x28
#define L3G_OUT_X_H 0x29
#define L3G_OUT_Y_L 0x2A
#define L3G_OUT_Y_H 0x2B
#define L3G_OUT_Z_L 0x2C
#define L3G_OUT_Z_H 0x2D

#define L3G_FIFO_CTRL_REG 0x2E
#define L3G_FIFO_SRC_REG 0x2F

#define L3G_INT1_CFG 0x30
#define L3G_INT1_SRC 0x31
#define L3G_INT1_THS_XH 0x32
#define L3G_INT1_THS_XL 0x33
#define L3G_INT1_THS_YH 0x34
#define L3G_INT1_THS_YL 0x35
#define L3G_INT1_THS_ZH 0x36
#define L3G_INT1_THS_ZL 0x37
#define L3G_INT1_DURATION 0x38

static ivector calib;

/**
 * Brings up the 9-DOF IMU in the default configuration.
 */
void imu9Init() {
	// Enable Accelerometer
	// 0x47 = 0b01000111
	// Normal power mode, all axes enabled, 50 Hz
	i2cWriteRegister(ACC_ADDRESS, LSM303_CTRL_REG1_A, 0x47);
	// High resolution output mode, 2 g full scale
	i2cWriteRegister(ACC_ADDRESS, LSM303_CTRL_REG4_A, 0x08);
	// Enable Magnetometer
	// 0x00 = 0b00000000
	// Continuous conversion mode
	i2cWriteRegister(MAG_ADDRESS, LSM303_MR_REG_M, 0x00);
	i2cWriteRegister(MAG_ADDRESS, LSM303_CRA_REG_M, 0x14);
	i2cWriteRegister(MAG_ADDRESS, LSM303_CRB_REG_M, 0x40);
	// 0x0F = 0b00001111
	// Normal power mode, all axes enabled, 100 Hz
	i2cWriteRegister(L3GD20_ADDRESS, L3G_CTRL_REG1, 0x0F);
	// 500 dps full scale
	i2cWriteRegister(L3GD20_ADDRESS, L3G_CTRL_REG4, 0x10);
}

/**
 * Reads raw data from the 9-DOF IMU.
 *
 * @param gyro the vector to return gyro values
 * @param accel the vector to return accelerometer values
 * @param mag the vector to return magnetometer values
 * @return whether the operation completed successfully
 */
bool imu9Raw(ivector *gyro, ivector *accel, ivector *mag) {
	uint8_t values[6];
	bool ok = true;
	if (i2cReadRegister(MAG_ADDRESS, LSM303_OUT_X_H_M, &values[0], 6)) {
		// Magnetometer
		mag->ix = ((int16_t)values[0] << 8) | (int16_t)values[1];
		mag->iy = ((int16_t)values[4] << 8) | (int16_t)values[5];
		mag->iz = ((int16_t)values[2] << 8) | (int16_t)values[3];
	} else
		ok = false;
	// Assert the MSB of the address to get the accelerometer to auto increment
	if (i2cReadRegister(ACC_ADDRESS, LSM303_OUT_X_L_A | 0x80, &values[0], 6)) {
		// Accelerometer
		accel->ix = (int16_t)(((int16_t)values[1] << 8) | (int16_t)values[0]) >> 4;
		accel->iy = (int16_t)(((int16_t)values[3] << 8) | (int16_t)values[2]) >> 4;
		accel->iz = (int16_t)(((int16_t)values[5] << 8) | (int16_t)values[4]) >> 4;
	} else
		ok = false;
	if (i2cReadRegister(L3GD20_ADDRESS, L3G_OUT_X_L | 0x80, &values[0], 6)) {
		// Gyro
		gyro->ix = ((int16_t)values[1] << 8) | (int16_t)values[0];
		gyro->iy = ((int16_t)values[3] << 8) | (int16_t)values[2];
		gyro->iz = ((int16_t)values[5] << 8) | (int16_t)values[4];
	} else
		ok = false;
	return ok;
}

/**
 * Calibrates the IMU's gyro.
 */
void imu9Calibrate(void) {
	// Calibrate gyro over a 0.5 second period
	int32_t totalX = 0, totalY = 0, totalZ = 0;
	ivector g, a, m;
	for (uint32_t i = 0; i < 64; i++) {
		// Average 256 readings
		imu9Raw(&g, &a, &m);
		totalX += g.ix;
		totalY += g.iy;
		totalZ += g.iz;
		msleep(15UL);
	}
	// Save calibration
	calib.ix = (totalX + 32) >> 6;
	calib.iy = (totalY + 32) >> 6;
	calib.iz = (totalZ + 32) >> 6;
}

/**
 * Reads calibrated data from the 9-DOF IMU.
 *
 * @param gyro the vector to return gyro values
 * @param accel the vector to return accelerometer values
 * @param mag the vector to return magnetometer values
 * @return whether the operation completed successfully
 */
bool imu9Read(vector *gyro, vector *accel, vector *mag) {
	ivector g, a, m;
	if (imu9Raw(&g, &a, &m)) {
		gyro->x = (float)((int)g.ix - (int)calib.ix);
		gyro->y = (float)((int)g.iy - (int)calib.iy);
		gyro->z = (float)((int)g.iz - (int)calib.iz);
		mag->x = (float)(int)m.ix - (0.5f * (M_X_MIN + M_X_MAX));
		mag->y = (float)(int)m.iy - (0.5f * (M_Y_MIN + M_Y_MAX));
		mag->z = (float)(int)m.iz - (0.5f * (M_Z_MIN + M_Z_MAX));
		accel->x = (float)(int)a.ix;
		accel->y = (float)(int)a.iy;
		accel->z = (float)(int)a.iz;
		return true;
	}
	return false;
}

// Function: a_pitch
// Purpose: This function returns accelerometer pitch value
float a_pitch(vector *a) {
	float ss;
	arm_sqrt_f32(a->y * a->y + a->z * a->z, &ss);
	return atan2f(-a->x, ss);
}

// Function: g_pr_yaw
// Purpose: Calculate gyro yaw rate given IMU data.
float g_pr_yaw(vector *g, float pitch, float roll) {
	// ... Apply formula from pdf to actually get inertial Z
	return g->z;
}

// Function: a_roll
// Purpose: Calculate roll information.
float a_roll(vector *a) {
	return atan2f(a->y, a->z);
}

// Function: m_pr_yaw
// Purpose: Calculate yaw given IMU data.
float m_pr_yaw(vector *m, float pitch, float roll, float yaw) {
	float sinRoll, cosRoll, sinPitch, cosPitch;

	// Calculate sin, cos of pitch and roll
	arm_sin_cos_f32(roll, &sinRoll, &cosRoll);
	arm_sin_cos_f32(-pitch, &sinPitch, &cosPitch);
	const float x = m->x * (M_X_MAX - M_X_MIN);
	const float y = m->y * (M_Y_MAX - M_Y_MIN);
	const float z = m->z * (M_Z_MAX - M_Z_MIN);
	// Calculate yaw from magnetometer
	return atan2f(z * sinRoll - y * cosRoll, x * cosPitch + y * sinPitch * sinRoll +
		z * sinPitch * cosRoll);
}
