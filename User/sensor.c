#include "sensor.h"

uint8_t arr[8] = {0};  //����������������

float imu_angle[3];	//�洢�Ƕ�ֵ

/* �������������� */
void imuUpdata(void) {
	nowtime++;
}


/* �ж�������Ԫ�� 1 �Ƿ����� */
uint8_t isOnesContinuous(uint8_t arr[], int size) {
    uint8_t hasSeenOne = 0;  // ����Ƿ��Ѿ������� 1
    uint8_t hasSeenZeroAfterOne = 0;  // ��������� 1 ֮���Ƿ������� 0

    for (int i = 0; i < 8; i++) {
        if (arr[i] == size) {
            if (hasSeenZeroAfterOne) {
                // ��������� 1 ֮���Ѿ������� 0��˵�� 1 ������
                return 0;
            }
            hasSeenOne = 1;
        } else if (hasSeenOne) {
            // ����Ѿ������� 1���������� 0�����Ϊ�Ѿ������� 0
            hasSeenZeroAfterOne = 1;
        }
    }
    return 1;
}

void get_sensor_value(uint8_t arr[]) {
    arr[0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);  // ��ȡ������ֵ
    arr[1] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
    arr[2] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5); 
    arr[3] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
    arr[4] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
    arr[5] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
    arr[6] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1);
    arr[7] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0);
}

/* ��ȡ���⴫����ƫ��ֵ */
int get_turn_error(void) {
    static int error_last = 0;
    int error = 0;
    int value = 1;  //�����������������򣬴�С����ǿ��

    get_sensor_value(arr);  // ��ȡ������ֵ

    if(isOnesContinuous(arr, 8)) {
        // ѭ��
        if(arr[0] == 0) {
            error+=4*value;
        }
        if(arr[1] == 0) {
            error+=3*value;
        }
        if(arr[2] == 0) {
            error+=2*value;
        }
        if(arr[3] == 0) {
            error+=value;
        }
        if(arr[4] == 0) {
            error-=value;
        }
        if(arr[5] == 0) {
            error-=2*value;
        }
        if(arr[6] == 0) {
            error-=3*value;
        }
        if(arr[7] == 0) {
            error-=4*value;
        }
        error_last = error;
    }
    else {
        error = error_last;
    }
    return error;
}