#ifndef __SENSOR__H
#define __SENSOR__H

#include "main.h"
#include "icm42688.h"
#include "IMU.h"
#include "usart.h"
#include "string.h"
#include "fsm.h"

extern uint8_t arr[8];
extern float imu_angle[3];	// ÍÓÂÝÒÇ×ËÌ¬½Ç¶ÈÖµ
extern uint16_t adcValue;

extern uint8_t visionBuf[10];
extern volatile uint8_t vision_rx_len;
extern volatile uint8_t NumNow[2];	// 0 : ×ó	1 : ÓÒ
extern uint16_t NumTimes[2][8];
extern uint8_t NumReal[2][2];	// 0 : ×ó	1 : ÓÒ
extern volatile uint8_t NumUpdate;

void imuUpdata(void);
int get_turn_error(void);
void get_sensor_value(uint8_t arr[]);
void VisionProc(void);
void ClearNum(void);

#endif

