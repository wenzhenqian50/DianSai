#ifndef __SENSOR__H
#define __SENSOR__H

#include "main.h"
#include "headfile.h"
#include "string.h"
#include "fsm.h"

extern float imu_angle[3];	// мсбщргвкл╛╫г╤хж╣
extern uint16_t adcValue;

void imuUpdata(void);
void CheckDrug(void);

#endif

