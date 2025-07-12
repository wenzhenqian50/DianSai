#include "task.h"

uint8_t task_num;  // ��������
uint32_t now_time; // ϵͳ��ǰʱ��

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

    sprintf(text,"Yaw:%.1f",imu_angle[0]);		// ��ʾʵʱ�Ƕ�
    lcd_print(&lcd_desc, 25, 25, text);

    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);   // ������

    sprintf(text,"click t27,1\xff\xff\xff");            // ����ָ��
    HAL_UART_Transmit(&huart2,(uint8_t *)text,14,100);  // ��������
    sprintf(text,"page0.t%d.txt=\"O--O\"\xff\xff\xff",SysParam.Local);         // ˢ��λ��
	if(SysParam.Local > 9) HAL_UART_Transmit(&huart2,(uint8_t *)text,23,100);  // ʮλ����
	else HAL_UART_Transmit(&huart2,(uint8_t *)text,22,100);                    // ��λ����

	// printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n",EventQueue.buffer[0],EventQueue.buffer[1],EventQueue.buffer[2],EventQueue.buffer[3],EventQueue.buffer[4],EventQueue.buffer[5],EventQueue.buffer[6],EventQueue.buffer[7],SysParam.Local);  // ��ӡ�¼�����
}

/* ---------- ������ ---------- */
static task_t scheduler_task[] = 
{
         // {MenuRun, 100, 0},	    // ��Ļ��ʾ
        {VisionProc,      50,   0},	// �Ӿ�ģ�����ݴ���
        {UpperUartTask,   20,   0},	// ��λ������
        {HsmDispatch,     10,   0},	// ϵͳ״̬�л�������
        {PackageDataProc, 10,   0}, // ��⳵������
        {Heart,           200,  0}, // ����ָʾ
        {CheckDrug,       500,  0}, // ���ҩƷ״̬
};


void SchedulerInit()
{
    task_num = sizeof(scheduler_task) / sizeof(task_t);
}

void SchedulerRun()
{
    for (uint8_t i = 0; i < task_num; i++)
    { 	// ����ÿ������
        now_time = HAL_GetTick();
        if (now_time >= scheduler_task[i].rate_ms + scheduler_task[i].last_run)
        {
            scheduler_task[i].last_run = now_time;
            scheduler_task[i].task_func();
        }
    }
}
