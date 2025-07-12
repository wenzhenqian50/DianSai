#include "track.h"

uint8_t arr[8] = {0};  	// 传感器的输入数组


/* 判断数组中元素 1 是否连续(判断是否到达末端斑马线) */
static uint8_t isOnesContinuous(uint8_t arr[], int size) {
    uint8_t hasSeenOne = 0;  		  // 标记是否已经遇到过 1
    uint8_t hasSeenZeroAfterOne = 0;  // 标记在遇到 1 之后是否遇到过 0

    for (int i = 0; i < size; i++) {
        if (arr[i] == 1) {
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

/* 检测路口函数 */
static uint8_t notCross(const uint8_t arr[], int size) {
    uint8_t count = 0;

    // 遍历数组的每个元素
    for (int i = 0; i < size; i++) {
        if (arr[i] == 1) {
            count++; // 如果元素是1，计数器加1
        }
        if (count >= 7) {
            return 1;
        }
    }

    // 检测T字路口
    if (arr[0] == 1 && arr[1] == 1 && arr[2] == 1 && arr[3] == 1) {
        return 2; // 满足条件，立即返回2
    }

    if (arr[4] == 1 && arr[5] == 1 && arr[6] == 1 && arr[7] == 1) {
        return 2; // 满足条件，立即返回2
    }

    return 0;
}

/* 读取灰度传感器数字量 */
void GetSensorValue(uint8_t arr[]) {
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
int GetTurnError(void) {
	static uint8_t cnt = 0;
	static uint8_t cross_flag = 0;
    Event event;
    int error = 0;
    int value = 1;  //正负调整传感器方向，大小调节强度
    int cross_r = 0;

    GetSensorValue(arr);  // 获取传感器值

    cross_r = notCross(arr, 8);

    if(cross_r == 0) {
         if(isOnesContinuous(arr, 8)) {
            if(arr[0] == 1) {
                error+=4*value;
            }
            if(arr[1] == 1) {
                error+=3*value;
            }
            if(arr[2] == 1) {
                error+=2*value;
            }
            if(arr[3] == 1) {
                error+=value;
            }
            if(arr[4] == 1) {
                error-=value;
            }
            if(arr[5] == 1) {
                error-=2*value;
            }
            if(arr[6] == 1) {
                error-=3*value;
            }
            if(arr[7] == 1) {
                error-=4*value;
            }
            // printf("run\n");
			cross_flag = 0;
        }
        else {
            // printf("end\n");
			cnt++;
			if(cnt >= 3) {     // 消抖
				event = ArriveEnd;
                queue_push(&EventQueue,event);
				cnt = 0;
			}
        }
    }
    else if(cross_r == 1) {
		// printf("cross\n");
		if(cross_flag == 0) {   // 防止重复触发
			event = PassCross;
            queue_push(&EventQueue,event);
			cross_flag = 1;
		}
    }
    else if(cross_r == 2) {
		// printf("cross_t\n");
		if(cross_flag == 0) {   // 防止重复触发
			event = PassCross_T;
            queue_push(&EventQueue,event);
			cross_flag = 1;
		}
    }

    return error;
}
