#include "vofa.h"

uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_len = 0;
volatile uint8_t uart_flag = 0;

float pid_p = 0.0f;
float pid_i = 0.0f;
float pid_d = 0.0f;

/* 处理串口缓冲区数据 */
void UpperUartTask(void) {
    // 检查是否有新的数据需要处理
    if (uart_flag) {
		// 在数据末尾添加 "\0"  防止字符串函数越界读取
        if (rx_len < RX_BUFFER_SIZE) {
            rx_buffer[rx_len] = '\0';
        } else {
            rx_buffer[RX_BUFFER_SIZE - 1] = '\0';
        }

        int   param_index = 0;
        float param_value = 0;

        //    使用sscanf进行强大且灵活的解析
        //    格式 "D%d:%f" 的含义:
        //    D      - 匹配字符 'D'
        //    %d     - 匹配一个整数 (参数索引)
        //    :      - 匹配字符 ':'
        //    %f     - 匹配一个浮点数 (参数值)
        // 	  sscanf会返回成功匹配的参数个数，这里期望是2个
        if (sscanf((const char*)rx_buffer, "D%d:%f", &param_index, &param_value) == 2) {
			
            switch (param_index) {
                case 1:
                    pid_p = param_value;
                    break;
                case 2:
                    pid_i = param_value;
                    break;
                case 3:
                    pid_d = param_value;
                    break;
                // case 4:
                //     Expand
				//     break;
                default:
                    printf("Error: Invalid parameter index\n");
            }
        } else {
            printf("Error: Invalid command format.\n");
        }

        memset(rx_buffer, 0, RX_BUFFER_SIZE);
        uart_flag = 0;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, RX_BUFFER_SIZE);
    }
}

#if 0

/* 串口空闲中断回调函数 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART1) {
        rx_len = Size;      // 记录本次接收到的数据长度
        uart_flag = 1;      // 设置标志位，通知主循环处理
							 // 不需要在这里重启串口接收
    }
}

#endif