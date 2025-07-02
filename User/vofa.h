#ifndef __VOFA__H
#define __VOFA__H

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "usart.h"

#define RX_BUFFER_SIZE 32

extern uint8_t           rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_len;
extern volatile uint8_t  uart_flag;

extern float pid_p;
extern float pid_i;
extern float pid_d;

void UpperUartTask(void);

#endif