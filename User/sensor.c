#include "sensor.h"

float imu_angle[3];		// 存储角度值
uint16_t adcValue = 0;	// 光线传感器


/* 陀螺仪心跳函数 */
void imuUpdata(void) {
	nowtime++;
}

/* 检测药品状态 */
void CheckDrug(void) {
	Event event;
	if(SysParam.Num != 0 && SysParam.Local == 0 && adcValue <= 20) {	// 必须先检测到目标病房号
		event = GetDrug;
		queue_push(&EventQueue, event);
	}
	if((SysParam.Local == 4 || SysParam.Local == 6 || SysParam.Local == 10 || SysParam.Local == 12 || SysParam.Local == 18 || SysParam.Local == 20 || SysParam.Local == 24 || SysParam.Local == 26) && adcValue > 20) {		// 必须到达目标病房号才能将药拿走
		event = LoseDrug;
		queue_push(&EventQueue, event);
	}
}


