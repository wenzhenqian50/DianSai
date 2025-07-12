#ifndef __SOFT_TIMER_H
#define __SOFT_TIMER_H

#include "headfile.h"

// ���������ʱ���ṹ��
typedef struct {
    uint8_t  active;           // ��ʱ������״̬: 1=����, 0=δ����
    uint32_t start_time;       // ��ʱ������ʱ��ϵͳTick
    uint32_t duration;         // ��Ҫ��ʱ��ʱ�� (��λ: ms)
    volatile uint8_t timeout;  // ��ʱ��־: 1=�ѳ�ʱ, 0=δ��ʱ (volatile����Ҫ��)
} SoftTimer_t;


extern SoftTimer_t SysTimer;


/**
  * @brief  ����һ�������ʱ�� (������)
  * @param  timer: ָ��Ҫ�����Ķ�ʱ���ṹ���ָ��
  * @param  duration_ms: ��Ҫ��ʱ�ĺ�����
  * @retval None
  */
void SoftTimer_Start(SoftTimer_t *timer, uint32_t duration_ms);

/**
  * @brief  ��鶨ʱ���Ƿ�ʱ����Ҫ����ѭ��Ƶ������
  * @param  timer: ָ��Ҫ�����Ķ�ʱ���ṹ���ָ��
  * @retval None
  */
void SoftTimer_Check(SoftTimer_t *timer);

/**
  * @brief  ֹͣһ�������ʱ��
  * @param  timer: ָ��Ҫ�����Ķ�ʱ���ṹ���ָ��
  * @retval None
  */
void SoftTimer_Stop(SoftTimer_t *timer);

#endif

