#ifndef __VOFA__H
#define __VOFA__H

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "usart.h"
#include "pid.h"

#define RX_BUFFER_SIZE 32

extern uint8_t           rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_len;
extern volatile uint8_t  uart_flag;

extern int testValue;

void UpperUartTask(void);

#endif