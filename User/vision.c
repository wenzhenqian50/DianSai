#include "vision.h"

volatile uint8_t NumNow[2] = {0};   // 当前从视觉模块接收的数字
uint8_t visionBuf[10] = {0};        // 视觉模块数据缓冲区
volatile uint8_t vision_rx_len = 0; // 保存视觉模块返回的数据长度
uint16_t NumTimes[2][8] = {0};      // 用于存储各数字出现的次数
uint8_t NumReal[2][2] = {0};        // 经过最大值滤波得到的实际检测值
volatile uint8_t NumUpdate = 0;     // 检测到视觉模块数据的标志
uint8_t TargetNum = 0;              // 用户指定病房号
uint16_t TargetNumTimes[8] = {0};   // 统计出现次数(用于消抖)
volatile uint8_t FarNum[4] = {0};   // 存储四个远端病房号
volatile uint8_t FarFarNum[2] = {0};// 存储两个远远病房号


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

/* 清除统计的数字出现次数数组 */
void ClearNum(void) {
	memset(NumTimes,0,sizeof(NumTimes));
}

/* 视觉模块数据解析 */
void VisionProc(void) {
    static uint8_t cnt = 0;
    // Event event;
	
    if(NumUpdate) {
        NumUpdate = 0;

		if(vision_rx_len >= 3) {
		   NumNow[0] = visionBuf[0] - 48;
		   NumNow[1] = visionBuf[2] - 48;

		   NumTimes[0][NumNow[0]-1]++;
		   NumTimes[1][NumNow[1]-1]++;

		   switch(FindMode) {
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
           // event = GetNum;
           // queue_push(&EventQueue,event);
		   // printf("%d\t%d,%d\t%d\n",NumReal[0][1],NumReal[0][0],NumReal[1][0],NumReal[1][1]);
		}
		else {
		   cnt++;
		   TargetNum = visionBuf[0] - 48;
		   TargetNumTimes[TargetNum - 1]++;
		   if(cnt >= 30) {      // 重复获取多次取出现次数最多的
			   SysParam.Num = 1 + FindMaxIndex(TargetNumTimes,8);
			   printf("Num:%d\n",SysParam.Num);
			   HAL_UART_Transmit(&huart6,(uint8_t *)"2",1,100);     // 将视觉模块切换为双数字中部病房识别模式
			   cnt = 0;
		   }
		}

		HAL_UARTEx_ReceiveToIdle_DMA(&huart6,visionBuf,10);
    }
}