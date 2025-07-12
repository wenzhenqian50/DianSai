#ifndef __SOFT_TIMER_H
#define __SOFT_TIMER_H

#include "headfile.h"

// 定义软件定时器结构体
typedef struct {
    uint8_t  active;           // 定时器激活状态: 1=激活, 0=未激活
    uint32_t start_time;       // 定时器启动时的系统Tick
    uint32_t duration;         // 需要延时的时长 (单位: ms)
    volatile uint8_t timeout;  // 超时标志: 1=已超时, 0=未超时 (volatile很重要！)
} SoftTimer_t;


extern SoftTimer_t SysTimer;


/**
  * @brief  启动一个软件定时器 (非阻塞)
  * @param  timer: 指向要操作的定时器结构体的指针
  * @param  duration_ms: 需要延时的毫秒数
  * @retval None
  */
void SoftTimer_Start(SoftTimer_t *timer, uint32_t duration_ms);

/**
  * @brief  检查定时器是否超时，需要被主循环频繁调用
  * @param  timer: 指向要操作的定时器结构体的指针
  * @retval None
  */
void SoftTimer_Check(SoftTimer_t *timer);

/**
  * @brief  停止一个软件定时器
  * @param  timer: 指向要操作的定时器结构体的指针
  * @retval None
  */
void SoftTimer_Stop(SoftTimer_t *timer);

#endif

