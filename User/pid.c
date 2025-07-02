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

/* ת��pid���� */
pid_t pid_turn ={	
	.Kp = 40,			
	.Ki = 0,				
	.Kd = 0,				
	.Target = 0,			
	.Measure = 0,			
	.Error = {0,0,0},				
	.KpOut = 0,				
	.KiOut = 0,				
	.KdOut = 0,				
	.PID_Out = 0,			
	.PID_Limit_MAX = 1000,	
	.Ki_Limit_MAX = 1000,	
};

/* �ǶȻ�pid���� */
pid_t pid_angle ={	
	.Kp = 1,			
	.Ki = 0,				
	.Kd = 3,				
	.Target = 0,			
	.Measure = 0,			
	.Error = {0,0,0},				
	.KpOut = 0,				
	.KiOut = 0,				
	.KdOut = 0,				
	.PID_Out = 0,			
	.PID_Limit_MAX = 9999,	
	.Ki_Limit_MAX = 9999,	
};


/* PID�޷����� */
static void PID_Limit(float *a, float ABS_MAX) {
    if (*a > ABS_MAX) *a = ABS_MAX;
    if (*a < -ABS_MAX) *a = -ABS_MAX;
}

/* ����ʽ PID ���㹫ʽ */
static float PID_Calculate(pid_t *pid, float Measure, float Target) {
	
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

    // return pid->PID_Out + 10.0f * Target + 150;		// ǰ�� + ��������
	return pid->PID_Out;
}


// **�Ƕ�������** (ȷ�����ʼ���� -180�� ~ 180��)
static float Compute_Angle_Error(float target_angle, float current_angle) {
    float error = target_angle - current_angle;
    return fmod(error + 180.0f, 360.0f) - 180.0f;
}

// ����ʽ PID �ǶȻ�����
static float PID_Angle_Calculate(pid_t *pid, float measure_angle, float target_angle) {
    pid->Target = target_angle;
    pid->Measure = measure_angle;

    // **����Ƕ����**
    pid->Error[0] = Compute_Angle_Error(target_angle, measure_angle);

    // ��������
    float delta_u = pid->Kp * (pid->Error[0] - pid->Error[1]) +
        pid->Ki * pid->Error[0] +
        pid->Kd * (pid->Error[0] - 2 * pid->Error[1] + pid->Error[2]);

    // ���� PID ���
    pid->PID_Out += delta_u;
    PID_Limit(&(pid->PID_Out), pid->PID_Limit_MAX); // �޷� PID ���

    // �����������
    pid->Error[2] = pid->Error[1];
    pid->Error[1] = pid->Error[0];

    return pid->PID_Out;
}


/* PWM�޷� */
static void pwm_limit(int *target, int MAX) {
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

/* �ײ��ٶȱջ����ƺ��� */
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

void PID_Angle(float angle) {
	int speed = PID_Angle_Calculate(&pid_angle,imu_angle[0],angle);
	PID_Speed(speed, -speed);
}



/* ����ܿ��� */
void MotorRun(void) {
	
	switch(State)  {
		case STOP: {
			
		}
		break;
		case WAIT: {
			
		}
		break;
		case TURN: {
			
		}
		break;
		case TRACK: {
			
		}
	}
}

