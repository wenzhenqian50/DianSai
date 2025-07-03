#ifndef __TASK__H
#define __TASK__H

#include "icm42688.h"
#include "main.h"
#include "menu.h"
#include "pid.h"
#include "vofa.h"


void SchedulerRun(void);
void SchedulerInit(void);

#endif
