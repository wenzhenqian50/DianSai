#include "soft_timer.h"


SoftTimer_t SysTimer;

/**
  * @brief  ����һ�������ʱ�� (������)
  * @param  timer: ָ��Ҫ�����Ķ�ʱ���ṹ���ָ��
  * @param  duration_ms: ��Ҫ��ʱ�ĺ�����
  * @retval None
  */
void SoftTimer_Start(SoftTimer_t *timer, uint32_t duration_ms)
{
    if (timer == NULL) return;

    timer->active     = 1;
    timer->duration   = duration_ms;
    timer->start_time = HAL_GetTick();
    timer->timeout    = 0; // ���ó�ʱ��־
}

/**
  * @brief  ��鶨ʱ���Ƿ�ʱ����Ҫ����ѭ��Ƶ������
  * @param  timer: ָ��Ҫ�����Ķ�ʱ���ṹ���ָ��
  * @retval None
  */
void SoftTimer_Check(SoftTimer_t *timer)
{
    if (timer == NULL || !timer->active) {
        return; 				// �����ʱ��δ���ֱ�ӷ���
    }

    if ((uwTick - timer->start_time) >= timer->duration) {
		    Event event;
        timer->active = 0;      // ֹͣ��ʱ��
        timer->timeout = 1;     // ���ó�ʱ��־ (�����¼�)
		    event = TimeOut;
		    queue_push(&EventQueue,event);
    }
}

/**
  * @brief  ֹͣһ�������ʱ��
  * @param  timer: ָ��Ҫ�����Ķ�ʱ���ṹ���ָ��
  * @retval None
  */
void SoftTimer_Stop(SoftTimer_t *timer)
{
    if (timer == NULL) return;
    timer->active = 0;
}