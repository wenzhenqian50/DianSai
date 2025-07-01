#ifndef __TASK__H
#define __TASK__H

#include "icm42688.h"
#include "shell.h"
#include "main.h"
#include "menu.h"
#include "pid.h"


void SchedulerRun(void);
void SchedulerInit(void);

#endif
