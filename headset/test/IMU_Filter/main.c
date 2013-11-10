/* Includes */
#include "main.h"
#include "core_cm4.h"
#include "printf.h"

unsigned volatile long sysTime = 0UL;

// Send print statements to the PC
int fputc(int c, FILE *stream) {
	while (!(USART3->SR & USART_FLAG_TXE));
	USART3->DR = (uint8_t)c;
	return c;
}

// Vector of integer values
typedef struct {
	int16_t ix, iy, iz;
} ivector;

// Addresses for the parts on the I2C bus
#define MAG_ADDRESS (0x3C >> 1)
#define ACC_ADDRESS (0x32 >> 1)
#define L3GD20_ADDRESS (0xD6 >> 1)

// LSM303 registers
#define LSM303_CTRL_REG1_A       0x20
#define LSM303_CTRL_REG2_A       0x21
#define LSM303_CTRL_REG3_A       0x22
#define LSM303_CTRL_REG4_A       0x23
#define LSM303_CTRL_REG5_A       0x24
#define LSM303_CTRL_REG6_A       0x25
#define LSM303_REFERENCE_A       0x26
#define LSM303_STATUS_REG_A      0x27

#define LSM303_OUT_X_L_A         0x28
#define LSM303_OUT_X_H_A         0x29
#define LSM303_OUT_Y_L_A         0x2A
#define LSM303_OUT_Y_H_A         0x2B
#define LSM303_OUT_Z_L_A         0x2C
#define LSM303_OUT_Z_H_A         0x2D

#define LSM303_FIFO_CTRL_REG_A   0x2E
#define LSM303_FIFO_SRC_REG_A    0x2F
#define LSM303_INT1_CFG_A        0x30
#define LSM303_INT1_SRC_A        0x31
#define LSM303_INT1_THS_A        0x32
#define LSM303_INT1_DURATION_A   0x33
#define LSM303_INT2_CFG_A        0x34
#define LSM303_INT2_SRC_A        0x35
#define LSM303_INT2_THS_A        0x36
#define LSM303_INT2_DURATION_A   0x37

#define LSM303_CLICK_CFG_A       0x38
#define LSM303_CLICK_SRC_A       0x39
#define LSM303_CLICK_THS_A       0x3A
#define LSM303_TIME_LIMIT_A      0x3B
#define LSM303_TIME_LATENCY_A    0x3C
#define LSM303_TIME_WINDOW_A     0x3D
#define LSM303_CRA_REG_M         0x00
#define LSM303_CRB_REG_M         0x01
#define LSM303_MR_REG_M          0x02

#define LSM303_OUT_X_H_M         0x03
#define LSM303_OUT_X_L_M         0x04
#define LSM303_OUT_Y_H_M         0x07
#define LSM303_OUT_Y_L_M         0x08
#define LSM303_OUT_Z_H_M         0x05
#define LSM303_OUT_Z_L_M         0x06
#define LSM303_SR_REG_M          0x09
#define LSM303_IRA_REG_M         0x0A
#define LSM303_IRB_REG_M         0x0B
#define LSM303_IRC_REG_M         0x0C

#define LSM303_TEMP_OUT_H_M      0x31
#define LSM303_TEMP_OUT_L_M      0x32

// L3G registers
#define L3G_WHO_AM_I      0x0F

#define L3G_CTRL_REG1     0x20
#define L3G_CTRL_REG2     0x21
#define L3G_CTRL_REG3     0x22
#define L3G_CTRL_REG4     0x23
#define L3G_CTRL_REG5     0x24
#define L3G_REFERENCE     0x25
#define L3G_OUT_TEMP      0x26
#define L3G_STATUS_REG    0x27

#define L3G_OUT_X_L       0x28
#define L3G_OUT_X_H       0x29
#define L3G_OUT_Y_L       0x2A
#define L3G_OUT_Y_H       0x2B
#define L3G_OUT_Z_L       0x2C
#define L3G_OUT_Z_H       0x2D

#define L3G_FIFO_CTRL_REG 0x2E
#define L3G_FIFO_SRC_REG  0x2F

#define L3G_INT1_CFG      0x30
#define L3G_INT1_SRC      0x31
#define L3G_INT1_THS_XH   0x32
#define L3G_INT1_THS_XL   0x33
#define L3G_INT1_THS_YH   0x34
#define L3G_INT1_THS_YL   0x35
#define L3G_INT1_THS_ZH   0x36
#define L3G_INT1_THS_ZL   0x37
#define L3G_INT1_DURATION 0x38

// Raw read
static void imu9Raw(ivector *gyro, ivector *accel, ivector *mag) {
	uint8_t values[6];
	if (i2cReadRegister(MAG_ADDRESS, LSM303_OUT_X_H_M, &values[0], 6)) {
		// Magnetometer
		mag->ix = ((int16_t)values[0] << 8) | (int16_t)values[1];
		mag->iy = ((int16_t)values[4] << 8) | (int16_t)values[5];
		mag->iz = ((int16_t)values[2] << 8) | (int16_t)values[3];
	}
	// Assert the MSB of the address to get the accelerometer to auto increment
	if (i2cReadRegister(ACC_ADDRESS, LSM303_OUT_X_L_A | 0x80, &values[0], 6)) {
		// Accelerometer
		accel->ix = (int16_t)(((int16_t)values[1] << 8) | (int16_t)values[0]) >> 4;
		accel->iy = (int16_t)(((int16_t)values[3] << 8) | (int16_t)values[2]) >> 4;
		accel->iz = (int16_t)(((int16_t)values[5] << 8) | (int16_t)values[4]) >> 4;
	}
	if (i2cReadRegister(L3GD20_ADDRESS, L3G_OUT_X_L | 0x80, &values[0], 6)) {
		// Gyro
		gyro->ix = ((int16_t)values[1] << 8) | (int16_t)values[0];
		gyro->iy = ((int16_t)values[3] << 8) | (int16_t)values[2];
		gyro->iz = ((int16_t)values[5] << 8) | (int16_t)values[4];
	}
}

// imu9Init - Brings up the 9DOF in default configuration
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

void msleep(unsigned long howLong) {
	unsigned long then = sysTime + howLong;
	while (sysTime < then) __WFI();
}

ivector g, a, m;

void init(void) {
	GPIO_InitTypeDef gp;
	USART_InitTypeDef us;
	USART_ClockInitTypeDef uc;
	// Tick every 1ms
	buttonInit(BUTTON_MODE_GPIO);
	ledInit();
	// USART 3 up
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	gp.GPIO_OType = GPIO_OType_PP;
	gp.GPIO_Speed = GPIO_PuPd_NOPULL;
	gp.GPIO_Mode = GPIO_Mode_AF;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gp);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	// USART 3 config
	us.USART_BaudRate = 115200;
	us.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	us.USART_Parity = USART_Parity_No;
	us.USART_StopBits = USART_StopBits_1;
	us.USART_WordLength = USART_WordLength_8b;
	// Half duplex TX only
	us.USART_Mode = USART_Mode_Tx;
	USART_Init(USART3, &us);
	// Clocking mode
	uc.USART_Clock = USART_Clock_Disable;
	uc.USART_LastBit = USART_LastBit_Disable;
	uc.USART_CPHA = USART_CPHA_1Edge;
	uc.USART_CPOL = USART_CPOL_High;
	USART_ClockInit(USART3, &uc);
	USART_Cmd(USART3, ENABLE);
	SysTick_Config(168000);
}

/* GPS
 * 	LAT: 113200 m/deg
 *  LNG:  78710 m/deg (variable)
 *
 * // GPS coordinates with respect to local 'origin', in meters
 * x_0 = GPS_getx();
 * y_0 = GPS_gety();
 *
 * while(1) {
 * 	x = (GPS_getx() - x_0) * GPS_UNIT_CONVERSION;
 * 	y = (GPS_gety() - y_0) * GPS_UNIT_CONVERSION;
 * }
 */

float m_hardiron_offset() {
	return 1;
}

float atan_alec(float arg) {
	return arg/(1 + .28125*arg*arg);
}

float fast_sqrt( float number )
{
        long i;
        float x2, y;
        const float threehalfs = 1.5F;

        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

        return 1/y;
}

float a_pitch(ivector a) {
	if ((a.iy == 0) && (a.iz == 0)) {
		return PI/2;
	}
	return atan2(-a.ix, fast_sqrt(a.iy*a.iy + a.iz*a.iz));
}

float a_roll(ivector a) {
	if (a.iz == 0) {
		return PI/2;
	}
	return atan2(a.iy, a.iz);
}

float m_pr_yaw(ivector m, float pitch, float roll) {
	//const float CONV_FACTOR = 1/(855*10000); // (1/855) -> Gauss; (1/10000) -> Tesla
	float yaw = (m.iz*sin(roll) - m.iy*cos(roll))/(m.ix*cos(pitch) + m.iy*sin(pitch)*sin(roll) + m.iz*sin(pitch)*cos(roll));
	if (yaw > 0) {
		while (yaw > PI) {
			yaw -= PI;
		}
	} else {
		while (yaw < -PI) {
			yaw += PI;
		}
	}
	return yaw;
}

float g_pr_pitch(ivector g, float pitch, float roll) {
	return 1;
}

float g_pyr_yaw(ivector g, float pitch, float yaw, float roll) {
	return 1;
}

float g_pr_roll(ivector g, float pitch, float roll) {
	return 1;
}

float get_avg(int length, int* values) {
	float sum = 0;
	for (int idx=0; idx<length; ++idx) {
		sum += values[idx];
	}
	return (float) (sum/((float)length));
}

int main(void) {
	init();
	msleep(500UL);
	i2cInit();
	imu9Init();
	// filter parameters
	const int GYRO_UPDATES = 10;
	const int GYRO_PERIOD_MS = 1;
	const float GYRO_UNIT_CONVERSION = 1;
	const int AVG_WINDOW_LENGTH = 3;
	const float WEIGHT_GYRO = .5;
	const float WEIGHT_AM = 1 - WEIGHT_GYRO;
	// init variables
	int gyro_update_ctr = 0;
	int avg_p[AVG_WINDOW_LENGTH];
	int avg_y[AVG_WINDOW_LENGTH];
	int avg_r[AVG_WINDOW_LENGTH];
	int avg_window_idx = 0;
	float g_x = 0;
	float g_y = 0;
	float g_z = 0;
	float p = 0;
	float y = 0;
	float r = 0;
	float ab = 5;//.23423425;
	int bs = 0;
	ledOn(LED6);
	while (1) {
		imu9Raw(&g, &a, &m);
//		p = get_pitch(a);
//		y = get_yaw(m);
//		r = get_roll(a);
//		for (gyro_update_ctr = 0; gyro_update_ctr < GYRO_UPDATES; ++gyro_update_ctr) {
//			avg_p[avg_window_idx] = get_pitch(a);
//			avg_y[avg_window_idx] = get_yaw(m);
//			avg_r[avg_window_idx] = get_roll(a);
//			++avg_window_idx;
//			if (avg_window_idx >= AVG_WINDOW_LENGTH) {
//				avg_window_idx = 0;
//			}
//// TODO: are these values needed, gyro returns angular velocity (therefore, just assume constant velocity over GYRO_PERIOD_MS)
//			g_x = g.ix;
//			g_y = g.iy;
//			g_z = g.iz;
//			msleep(GYRO_PERIOD_MS);
//			imu9Raw(&g, &a, &m);
//// TODO: determine how to calculate pitch_yaw_roll from gyro differences
//			p += (g.iy - g_y)*GYRO_UNIT_CONVERSION*(float)(GYRO_PERIOD_MS/1000.);
//			y += (g.ix - g_x)*GYRO_UNIT_CONVERSION*(float)(GYRO_PERIOD_MS/1000.);
//			r += (g.iz - g_z)*GYRO_UNIT_CONVERSION*(float)(GYRO_PERIOD_MS/1000.);
//		}
//		p = WEIGHT_GYRO*p + WEIGHT_AM*get_avg(AVG_WINDOW_LENGTH, avg_p);
//		y = WEIGHT_GYRO*y + WEIGHT_AM*get_avg(AVG_WINDOW_LENGTH, avg_y);
//		r = WEIGHT_GYRO*r + WEIGHT_AM*get_avg(AVG_WINDOW_LENGTH, avg_r);
		//printf("P:%f\tY:%f\tR:%f\r\n", p, y, r);
		printf("Ax:%d\tAy:%d\tAz:%d\r\n", a.ix, a.iy, a.iz);
		printf("Gx:%f\tGy:%f\tGz:%f\r\n", (g.ix*17.5/1000.), (g.iy*17.5/1000.), (g.iz*17.5/1000.));
		printf("Mx:%f\tMy:%f\tMz:%f\r\n", (m.ix/855.*100000.), (m.iy/855.*100000.), (m.iz/760.*100000.));
		printf("Pitch:%f\tYaw:%f\tRoll:%f\r\n", a_pitch(a)*180./PI, m_pr_yaw(m, a_pitch(a), a_roll(a))*180./PI, a_roll(a)*180./PI);

		msleep(500UL);

//		printf("%d %d %d\r\n", (int)m.ix, (int)m.iy, (int)m.iz);
//		ledToggle(LED6);
//		if (a.ix > 0)
//			ledOff(LED3);
//		else
//			ledOn(LED3);
//		if (a.iy > 0)
//			ledOff(LED4);
//		else
//			ledOn(LED4);
//		if (a.iz > 0)
//			ledOff(LED5);
//		else
//			ledOn(LED5);
	}
	return 0;
}
