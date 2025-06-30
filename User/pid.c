#include "pid.h"

#define MAX_PWM 999		// ������PWM�޷�

/* �ҵ���ٶȻ�pid���� */
pid_t pid_r ={	
	.Kp = 0,			
	.Ki = 0.1,				
	.Kd = 0,				
	.Target = 0,			
	.Measure = 0,			
	.Error = {0,0,0},				
	.KpOut = 0,				
	.KiOut = 0,				
	.KdOut = 0,				
	.PID_Out = 0,			
	.PID_Limit_MAX = 999,	
	.Ki_Limit_MAX = 999,	
};

/* �����ٶȻ�pid���� */
pid_t pid_l ={	
	.Kp = 0,			
	.Ki = 0.1,				
	.Kd = 0,				
	.Target = 0,			
	.Measure = 0,			
	.Error = {0,0,0},				
	.KpOut = 0,				
	.KiOut = 0,				
	.KdOut = 0,				
	.PID_Out = 0,			
	.PID_Limit_MAX = 999,	
	.Ki_Limit_MAX = 999,	
};


/* PID�޷����� */
void PID_Limit(float *a, float ABS_MAX) {
    if (*a > ABS_MAX) *a = ABS_MAX;
    if (*a < -ABS_MAX) *a = -ABS_MAX;
}

/* ����ʽPID���㹫ʽ */
float PID_Calculate(pid_t *pid, float Measure, float Target) {
	
    pid->Target = Target;  // Ŀ��ֵ
    pid->Measure = Measure; // ����ֵ
    pid->Error[0] = Target - Measure; // ���㵱ǰ���

    // ��������
    float delta_u = pid->Kp * (pid->Error[0] - pid->Error[1]) +
                    pid->Ki * pid->Error[0] +
                    pid->Kd * (pid->Error[0] - 2 * pid->Error[1] + pid->Error[2]);

	
    // ����PID���
    pid->PID_Out += delta_u;
    PID_Limit(&(pid->PID_Out), pid->PID_Limit_MAX); // �޷�PID���

    // �����������
    pid->Error[2] = pid->Error[1];
    pid->Error[1] = pid->Error[0];

    return pid->PID_Out + 10.0f * Target + 150;		// ǰ�� + ��������
}


/* PWM�޷� */
void pwm_limit(int *target, int MAX) {
	if( *target > MAX) *target = MAX;
	else if(*target < (-MAX)) *target = -MAX;
}

/* ���������ٶ� */
void set_motor_left_speed(int target_pwm) {
    pwm_limit(&target_pwm,MAX_PWM);
    
    if(target_pwm >= 0) {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
	    TIM3->CCR1 = target_pwm;
    }
    else if(target_pwm < 0) {
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
		TIM3->CCR1 = -target_pwm;
    }
}

/* �����ҵ���ٶ� */
void set_motor_right_speed(int target_pwm) {
    pwm_limit(&target_pwm,MAX_PWM);
    
    if(target_pwm >= 0) {
        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);
	    TIM3->CCR2 = target_pwm;
    }
    else if(target_pwm < 0) {
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_SET);
		TIM3->CCR2 = -target_pwm;
    }
}

void PID_Speed(int speed_l, int speed_r) {
	// ��ȡ����������
	short count_l = (short)(__HAL_TIM_GET_COUNTER(&htim2));
	short count_r = (short)(__HAL_TIM_GET_COUNTER(&htim4));
	TIM2->CNT = 0;
	TIM4->CNT = 0;
	// �����ٶ�
	set_motor_left_speed(PID_Calculate(&pid_l,count_l,speed_l));
	set_motor_right_speed(PID_Calculate(&pid_r,count_r,speed_r));
}

