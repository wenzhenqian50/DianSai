#include "sensor.h"

uint8_t arr[8] = {0};  	// 传感器的输入数组

float imu_angle[3];		// 存储角度值

uint16_t adcValue = 0;	// 光线传感器

volatile uint8_t NumNow[2] = {0};
uint8_t visionBuf[10] = {0};
volatile uint8_t vision_rx_len = 0;
uint16_t NumTimes[2][8] = {0};
uint8_t NumReal[2][2] = {0};
volatile uint8_t NumUpdate = 0;
uint8_t TargetNum = 0;  // 用户指定病房号
uint16_t TargetNumTimes[8] = {0}; // 统计出现次数(用于消抖)


/* 陀螺仪心跳函数 */
void imuUpdata(void) {
	nowtime++;
}


/* 判断数组中元素 1 是否连续 */
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

static uint8_t notEnd(const uint8_t arr[], int size) {
    uint8_t count = 0;
    // 遍历数组的每个元素
    for (int i = 0; i < size; i++) {
        if (arr[i] == 1) {
            count++; // 如果元素是1，计数器加1
        }
        if (count >= 7) {
            return 0;
        }
    }
    return 1;
}

/* 读取灰度传感器数字量 */
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

    if(notEnd(arr, 8)) {
         if(isOnesContinuous(arr, 8)) {
            // 循迹
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
            error_last = error;
            // printf("run\n");
        }
        else {
            // printf("end\n");
            error = error_last;
            Event = End;
        }
    }
    else {
		// printf("cross\n");
        Event = Cross;
    }
    return error;
}

/* 查找一个整型数组中最大元素的索引 */
static int FindMaxIndex(const uint16_t arr[], int size) {
    if (arr == NULL || size <= 0) {
        return -1;
    }

    int max_index = 0;
    for (int i = 1; i < size; i++) {
        if (arr[i] > arr[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}

/* 清除原来统计的数字出现次数 */
void ClearNum(void) {
	memset(NumTimes,0,sizeof(NumTimes));
}

/* 视觉模块数据解析 */
void VisionProc(void) {
    static uint8_t cnt = 0;
	
    if(NumUpdate) {
        NumUpdate = 0;
        
		if(vision_rx_len >= 3) {
		   NumNow[0] = visionBuf[0] - 48;
		   NumNow[1] = visionBuf[2] - 48;

		   NumTimes[0][NumNow[0]-1]++;
		   NumTimes[1][NumNow[1]-1]++;

		   switch(Mode) {
			   case L: {
				   NumReal[0][1] = 1 + FindMaxIndex(NumTimes[0], 8);
				   NumReal[0][0] = 1 + FindMaxIndex(NumTimes[1], 8);
			   }
			   break;
			   case M: {
				   NumReal[0][0] = 1 + FindMaxIndex(NumTimes[0], 8);
				   NumReal[1][0] = 1 + FindMaxIndex(NumTimes[1], 8);
			   }
			   break;
			   case R: {
				   NumReal[1][0] = 1 + FindMaxIndex(NumTimes[0], 8);
				   NumReal[1][1] = 1 + FindMaxIndex(NumTimes[1], 8);
			   }
		   }
		   // printf("L:%d,R:%d\n",NumReal[0][0],NumReal[1][0]);
		}
		else {
		   cnt++;
		   TargetNum = visionBuf[0] - 48;
		   TargetNumTimes[TargetNum - 1]++;
		   if(cnt >= 10) {
			   Param.Num = 1 + FindMaxIndex(TargetNumTimes,8);
			   HAL_UART_Transmit(&huart6,(uint8_t *)"1",1,100);     // 将视觉模块切换为双数字识别模式
			   Event = GetNumber;
			   cnt = 0;
		   }
		   // printf("Num:%d\n",Param.Num);
		}
		
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6,visionBuf,10);
    }
}
