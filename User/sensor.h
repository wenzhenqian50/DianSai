#ifndef __SENSOR__H
#define __SENSOR__H

#include "main.h"
#include "icm42688.h"
#include "IMU.h"

extern uint8_t arr[8];
extern float imu_angle[3];	// ��������̬�Ƕ�ֵ

void imuUpdata(void);
int get_turn_error(void);
void get_sensor_value(uint8_t arr[]);

#endif

