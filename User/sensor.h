#ifndef __SENSOR__H
#define __SENSOR__H

#include "main.h"
#include "icm42688.h"
#include "IMU.h"


extern float imu_angle[3];	// ÍÓÂİÒÇ×ËÌ¬½Ç¶ÈÖµ

extern float gyro_offset_x;	// ÍÓÂİÒÇÁãÆ«Öµ
extern float gyro_offset_y;
extern float gyro_offset_z;

void imuUpdata(void);

void calibrate_gyro(uint16_t sample_count);
void read_gyro_corrected(float* cgx, float* cgy, float* cgz);

#endif

