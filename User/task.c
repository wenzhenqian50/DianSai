#include "task.h"

uint8_t task_num;  // 任务数量
uint32_t now_time; // 系统当前时间

typedef struct
{
    void (*task_func)(void);
    uint32_t rate_ms;
    uint32_t last_run;
} task_t;

static void Heart(void)
{
    char text[30] = {0};
	
	// printf("%d\n",adcValue);

    sprintf(text,"Yaw:%.1f",imu_angle[0]);		// 显示实时角度
    lcd_print(&lcd_desc, 25, 25, text);

    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);   // 心跳灯

    sprintf(text,"click t27,1\xff\xff\xff");            // 清屏指令
    HAL_UART_Transmit(&huart2,(uint8_t *)text,14,100);  // 发送清屏
    sprintf(text,"page0.t%d.txt=\"O--O\"\xff\xff\xff",SysParam.Local);         // 刷新位置
	if(SysParam.Local > 9) HAL_UART_Transmit(&huart2,(uint8_t *)text,23,100);  // 十位发送
	else HAL_UART_Transmit(&huart2,(uint8_t *)text,22,100);                    // 个位发送

	// printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n",EventQueue.buffer[0],EventQueue.buffer[1],EventQueue.buffer[2],EventQueue.buffer[3],EventQueue.buffer[4],EventQueue.buffer[5],EventQueue.buffer[6],EventQueue.buffer[7],SysParam.Local);  // 打印事件队列
}

/* ---------- 任务组 ---------- */
static task_t scheduler_task[] = 
{
         // {MenuRun, 100, 0},	    // 屏幕显示
        {VisionProc,      50,   0},	// 视觉模块数据处理
        {UpperUartTask,   20,   0},	// 上位机调参
        {HsmDispatch,     10,   0},	// 系统状态切换与运行
        {PackageDataProc, 10,   0}, // 监测车二数据
        {Heart,           200,  0}, // 心跳指示
        {CheckDrug,       500,  0}, // 检测药品状态
};


void SchedulerInit()
{
    task_num = sizeof(scheduler_task) / sizeof(task_t);
}

void SchedulerRun()
{
    for (uint8_t i = 0; i < task_num; i++)
    { 	// 遍历每个任务
        now_time = HAL_GetTick();
        if (now_time >= scheduler_task[i].rate_ms + scheduler_task[i].last_run)
        {
            scheduler_task[i].last_run = now_time;
            scheduler_task[i].task_func();
        }
    }
}
