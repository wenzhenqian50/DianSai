#include "task.h"

uint8_t task_num;	//��������
uint32_t now_time;	//ϵͳ��ǰʱ��

typedef struct {
    void (*task_func)(void);
    uint32_t rate_ms;
    uint32_t last_run;
}task_t;

static void Heart(void) {
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_0);
}

static task_t scheduler_task[] = 	//��������
{
	{MenuRun ,      100, 0},
	{VisionProc,    50,  0},
    {UpperUartTask, 20,  0},
    {SystemRun,     20,  0},
	{Heart,			500, 0},
};

void SchedulerInit() {
    task_num = sizeof(scheduler_task) / sizeof(task_t);
}

void SchedulerRun() {
    for(uint8_t i = 0; i < task_num; i++) {		//����ÿ������
        now_time = HAL_GetTick();
        if(now_time >= scheduler_task[i].rate_ms + scheduler_task[i].last_run) {
            scheduler_task[i].last_run = now_time;
            scheduler_task[i].task_func();
        }
    }
}





