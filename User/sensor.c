#include "sensor.h"

float imu_angle[3];		// �洢�Ƕ�ֵ
uint16_t adcValue = 0;	// ���ߴ�����


/* �������������� */
void imuUpdata(void) {
	nowtime++;
}

/* ���ҩƷ״̬ */
void CheckDrug(void) {
	Event event;
	if(SysParam.Num != 0 && SysParam.Local == 0 && adcValue <= 20) {	// �����ȼ�⵽Ŀ�겡����
		event = GetDrug;
		queue_push(&EventQueue, event);
	}
	if((SysParam.Local == 4 || SysParam.Local == 6 || SysParam.Local == 10 || SysParam.Local == 12 || SysParam.Local == 18 || SysParam.Local == 20 || SysParam.Local == 24 || SysParam.Local == 26) && adcValue > 20) {		// ���뵽��Ŀ�겡���Ų��ܽ�ҩ����
		event = LoseDrug;
		queue_push(&EventQueue, event);
	}
}


