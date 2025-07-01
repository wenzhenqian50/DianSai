#include "sensor.h"

uint8_t arr[8] = {0};  //传感器的输入数组

float imu_angle[3];	//存储角度值

/* 陀螺仪心跳函数 */
void imuUpdata(void) {
	nowtime++;
}


/* 判断数组中元素 1 是否连续 */
uint8_t isOnesContinuous(uint8_t arr[], int size) {
    uint8_t hasSeenOne = 0;  // 标记是否已经遇到过 1
    uint8_t hasSeenZeroAfterOne = 0;  // 标记在遇到 1 之后是否遇到过 0

    for (int i = 0; i < 8; i++) {
        if (arr[i] == size) {
            if (hasSeenZeroAfterOne) {
                // 如果在遇到 1 之后已经遇到过 0，说明 1 不连续
                return 0;
            }
            hasSeenOne = 1;
        } else if (hasSeenOne) {
            // 如果已经遇到过 1，现在遇到 0，标记为已经遇到过 0
            hasSeenZeroAfterOne = 1;
        }
    }
    return 1;
}

void get_sensor_value(uint8_t arr[]) {
    arr[0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);  // 读取传感器值
    arr[1] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
    arr[2] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5); 
    arr[3] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
    arr[4] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
    arr[5] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
    arr[6] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1);
    arr[7] = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0);
}

/* 获取红外传感器偏移值 */
int get_turn_error(void) {
    static int error_last = 0;
    int error = 0;
    int value = 1;  //正负调整传感器方向，大小调节强度

    get_sensor_value(arr);  // 获取传感器值

    if(isOnesContinuous(arr, 8)) {
        // 循迹
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