#include "vofa.h"

uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_len = 0;
volatile uint8_t uart_flag = 0;

float pid_p = 0.0f;
float pid_i = 0.0f;
float pid_d = 0.0f;

/* �����ڻ��������� */
void UpperUartTask(void) {
    // ����Ƿ����µ�������Ҫ����
    if (uart_flag) {
		// ������ĩβ��� "\0"  ��ֹ�ַ�������Խ���ȡ
        if (rx_len < RX_BUFFER_SIZE) {
            rx_buffer[rx_len] = '\0';
        } else {
            rx_buffer[RX_BUFFER_SIZE - 1] = '\0';
        }

        int   param_index = 0;
        float param_value = 0;

        //    ʹ��sscanf����ǿ�������Ľ���
        //    ��ʽ "D%d:%f" �ĺ���:
        //    D      - ƥ���ַ� 'D'
        //    %d     - ƥ��һ������ (��������)
        //    :      - ƥ���ַ� ':'
        //    %f     - ƥ��һ�������� (����ֵ)
        // 	  sscanf�᷵�سɹ�ƥ��Ĳ�������������������2��
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

/* ���ڿ����жϻص����� */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART1) {
        rx_len = Size;      // ��¼���ν��յ������ݳ���
        uart_flag = 1;      // ���ñ�־λ��֪ͨ��ѭ������
							 // ����Ҫ�������������ڽ���
    }
}

#endif