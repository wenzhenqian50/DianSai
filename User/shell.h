#ifndef __SHELL__H
#define __SHELL__H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include "usart.h"
#include "main.h"
#include "adc.h"

extern bool CmdReady;

void ShellInit(void);
void ShellRun(void); // 在主循环中调用的处理函数

#endif

