#include "soft_timer.h"


SoftTimer_t SysTimer;

/**
  * @brief  启动一个软件定时器 (非阻塞)
  * @param  timer: 指向要操作的定时器结构体的指针
  * @param  duration_ms: 需要延时的毫秒数
  * @retval None
  */
void SoftTimer_Start(SoftTimer_t *timer, uint32_t duration_ms)
{
    if (timer == NULL) return;

    timer->active     = 1;
    timer->duration   = duration_ms;
    timer->start_time = HAL_GetTick();
    timer->timeout    = 0; // 重置超时标志
}

/**
  * @brief  检查定时器是否超时，需要被主循环频繁调用
  * @param  timer: 指向要操作的定时器结构体的指针
  * @retval None
  */
void SoftTimer_Check(SoftTimer_t *timer)
{
    if (timer == NULL || !timer->active) {
        return; 				// 如果定时器未激活，直接返回
    }

    if ((uwTick - timer->start_time) >= timer->duration) {
		    Event event;
        timer->active = 0;      // 停止定时器
        timer->timeout = 1;     // 设置超时标志 (发出事件)
		    event = TimeOut;
		    queue_push(&EventQueue,event);
    }
}

/**
  * @brief  停止一个软件定时器
  * @param  timer: 指向要操作的定时器结构体的指针
  * @retval None
  */
void SoftTimer_Stop(SoftTimer_t *timer)
{
    if (timer == NULL) return;
    timer->active = 0;
}