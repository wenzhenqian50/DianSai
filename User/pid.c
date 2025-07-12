#include "pid.h"

#define MAX_PWM 999		// ������PWM�޷�

volatile float TargetAngle = 0;
volatile int CountAll = 0;		// ÿ�ε���PID_DistanceǰҪ��count_all����
volatile float Angle_d = 0;		// ����Ϊ����ʱ�ĽǶ�
volatile int Length = 0;		// �����н�������

/* �ҵ���ٶȻ�pid���� */
pid_t pid_r ={	
	.Kp = 21,			
	.Ki = 7.7,				
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
	.Kp = 21,			
	.Ki = 7.7,				
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

/* ���뻷pid���� */
pid_t pid_d ={	
	.Kp = 0.5,			
	.Ki = 0.01,				
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

/* ����ת��pid���� */
pid_t pid_turn_d ={	
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
	.PID_Limit_MAX = 999,	
	.Ki_Limit_MAX = 999,	
};

/* ת��pid���� */
pid_t pid_turn ={	
	.Kp = 50,			
	.Ki = 0,				
	.Kd = 20,				
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

/* �ǶȻ�pid���� */
pid_t pid_angle ={	
	.Kp = 0.8,			
	.Ki = 0,				
	.Kd = 2,				
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


/* �Ƕ�������(ȷ�����ʼ���� -180��~ 180��) */
static float Compute_Angle_Error(float target_angle, float current_angle) {
    float error = target_angle - current_angle;

    // ������һ���� -180 ~ 180 ��֮��
    while (error > 180.0f) {
        error -= 360.0f;
    }
    while (error < -180.0f) {
        error += 360.0f;
    }
    return error;
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
	
	// ����
	// printf("%d\n",count_l);
	// printf("%d\n",count_r);
	
	// �����ٶ�
	set_motor_left_speed(PID_Calculate(&pid_l,count_l,speed_l));
	set_motor_right_speed(PID_Calculate(&pid_r,count_r,speed_r));
}

/* �Ƕȿ��ƺ��� */
void PID_Angle(float angle) {
	int speed = PID_Angle_Calculate(&pid_angle,imu_angle[0],angle);
	PID_Speed(speed, -speed);
}

/* ת����ƺ��� */
void PID_Turn(int speed) {
	int error = GetTurnError();
	PID_Speed(speed + error, speed - error);
	// printf("%d\n",error);
}


/* �н�������� */
void PID_Distance(void) {
	int speed = 0;
	int error = 0;
	short count_l = (short)(__HAL_TIM_GET_COUNTER(&htim2));
	short count_r = (short)(__HAL_TIM_GET_COUNTER(&htim4));
	TIM2->CNT = 0;
	TIM4->CNT = 0;
	
	CountAll += (count_l + count_r) / 2;
	speed = PID_Calculate(&pid_d, CountAll, Length);
	
//	error = PID_Calculate(&pid_turn_d,imu_angle[0],Angle_d);

	// �����ٶ�
	set_motor_left_speed(speed);
	set_motor_right_speed(speed);
}

/* ���þ��뻷���� */
void SetDistanceParam(int length) {
	Length = length;
	Angle_d = imu_angle[0];
	CountAll = 0;
}

/* ����PID�������ڲ�״̬ */
static void PID_Reset(pid_t *pid) {
    pid->Target = 0;
    pid->Measure = 0;
    pid->Error[0] = 0;
    pid->Error[1] = 0;
    pid->Error[2] = 0;
    pid->KpOut = 0;
    pid->KiOut = 0;
    pid->KdOut = 0;
    pid->PID_Out = 0;
}

/* ת��Ƕ����� */
void SetTurnAngle(float angle) {
	// ���ýǶ�PID������, ��ֹ��ֵӰ��
	PID_Reset(&pid_angle); 
	
	TargetAngle = imu_angle[0] + angle;
	
	// ��һ��Ŀ��Ƕ�
	while (TargetAngle > 180.0f) TargetAngle -= 360.0f;
	while (TargetAngle <= -180.0f) TargetAngle += 360.0f;
}

/* ����ܿ���(��ʱ�жϵ���) */
void MotorRun(void) {
	static uint16_t turn_done_cnt = 0;
	static uint16_t move_done_cnt = 0;
	Event event;
	switch(MotionState)  {
		case STOP: {
			PID_Speed(0, 0);
		}
		break;
		case TURN: {
			PID_Angle(TargetAngle);
			
			if (fabs(pid_angle.Error[0]) < 3.0f) { // �ɵ�������ֵ
				turn_done_cnt++;
			} 
			else {
				turn_done_cnt = 0;  // �������������ü�����
			}
			
			if (turn_done_cnt >= 30) { 
				turn_done_cnt = 0;
				MotionState = STOP; // ת��������л���STOP״̬
				event = TurnDone;
				queue_push(&EventQueue,event);
			}
		}
		break;
		case TRACK: {
			if(TrackSpeed == SLOW) {
				PID_Turn(10);
			}
			else if(TrackSpeed == MID) {
				PID_Turn(15);
			}
			else if(TrackSpeed == FAST) {
				PID_Turn(25);
			}
		}
		break;
		case MOVE: {
			PID_Distance();
			move_done_cnt++;
			if(move_done_cnt >= 200) {
				move_done_cnt = 0;
				event = MoveDone;
				queue_push(&EventQueue,event);
			}
		}
	}
}

