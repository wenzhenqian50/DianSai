#include "pid.h"

#define MAX_PWM 999		// 输出电机PWM限幅

volatile float TargetAngle = 0;
volatile int CountAll = 0;		// 每次调用PID_Distance前要将count_all清零
volatile float Angle_d = 0;		// 设置为启动时的角度
volatile int Length = 0;		// 设置行进脉冲数

/* 右电机速度环pid参数 */
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

/* 左电机速度环pid参数 */
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

/* 距离环pid参数 */
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

/* 距离转向环pid参数 */
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

/* 转向环pid参数 */
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

/* 角度环pid参数 */
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


/* PID限幅函数 */
static void PID_Limit(float *a, float ABS_MAX) {
    if (*a > ABS_MAX) *a = ABS_MAX;
    if (*a < -ABS_MAX) *a = -ABS_MAX;
}

/* 增量式 PID 计算公式 */
static float PID_Calculate(pid_t *pid, float Measure, float Target) {
	
    pid->Target = Target;  // 目标值
    pid->Measure = Measure; // 测量值
    pid->Error[0] = Target - Measure; // 计算当前误差

    // 计算增量
    float delta_u = pid->Kp * (pid->Error[0] - pid->Error[1]) +
                    pid->Ki * pid->Error[0] +
                    pid->Kd * (pid->Error[0] - 2 * pid->Error[1] + pid->Error[2]);

	
    // 更新PID输出
    pid->PID_Out += delta_u;
    PID_Limit(&(pid->PID_Out), pid->PID_Limit_MAX); // 限幅PID输出

    // 更新误差数组
    pid->Error[2] = pid->Error[1];
    pid->Error[1] = pid->Error[0];

    // return pid->PID_Out + 10.0f * Target + 150;		// 前馈 + 死区补偿
	return pid->PID_Out;
}


/* 角度误差计算(确保误差始终在 -180°~ 180°) */
static float Compute_Angle_Error(float target_angle, float current_angle) {
    float error = target_angle - current_angle;

    // 将误差归一化到 -180 ~ 180 度之间
    while (error > 180.0f) {
        error -= 360.0f;
    }
    while (error < -180.0f) {
        error += 360.0f;
    }
    return error;
}

// 增量式 PID 角度环计算
static float PID_Angle_Calculate(pid_t *pid, float measure_angle, float target_angle) {
    pid->Target = target_angle;
    pid->Measure = measure_angle;

    // **计算角度误差**
    pid->Error[0] = Compute_Angle_Error(target_angle, measure_angle);

    // 计算增量
    float delta_u = pid->Kp * (pid->Error[0] - pid->Error[1]) +
        pid->Ki * pid->Error[0] +
        pid->Kd * (pid->Error[0] - 2 * pid->Error[1] + pid->Error[2]);

    // 更新 PID 输出
    pid->PID_Out += delta_u;
    PID_Limit(&(pid->PID_Out), pid->PID_Limit_MAX); // 限幅 PID 输出

    // 更新误差数组
    pid->Error[2] = pid->Error[1];
    pid->Error[1] = pid->Error[0];

    return pid->PID_Out;
}


/* PWM限幅 */
static void pwm_limit(int *target, int MAX) {
	if( *target > MAX) *target = MAX;
	else if(*target < (-MAX)) *target = -MAX;
}

/* 设置左电机速度 */
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

/* 设置右电机速度 */
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

/* 底层速度闭环控制函数 */
void PID_Speed(int speed_l, int speed_r) {
	// 获取编码器读数
	short count_l = (short)(__HAL_TIM_GET_COUNTER(&htim2));
	short count_r = (short)(__HAL_TIM_GET_COUNTER(&htim4));
	TIM2->CNT = 0;
	TIM4->CNT = 0;
	
	// 调试
	// printf("%d\n",count_l);
	// printf("%d\n",count_r);
	
	// 设置速度
	set_motor_left_speed(PID_Calculate(&pid_l,count_l,speed_l));
	set_motor_right_speed(PID_Calculate(&pid_r,count_r,speed_r));
}

/* 角度控制函数 */
void PID_Angle(float angle) {
	int speed = PID_Angle_Calculate(&pid_angle,imu_angle[0],angle);
	PID_Speed(speed, -speed);
}

/* 转向控制函数 */
void PID_Turn(int speed) {
	int error = GetTurnError();
	PID_Speed(speed + error, speed - error);
	// printf("%d\n",error);
}


/* 行进距离控制 */
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

	// 设置速度
	set_motor_left_speed(speed);
	set_motor_right_speed(speed);
}

/* 设置距离环参数 */
void SetDistanceParam(int length) {
	Length = length;
	Angle_d = imu_angle[0];
	CountAll = 0;
}

/* 重置PID控制器内部状态 */
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

/* 转向角度设置 */
void SetTurnAngle(float angle) {
	// 重置角度PID控制器, 防止旧值影响
	PID_Reset(&pid_angle); 
	
	TargetAngle = imu_angle[0] + angle;
	
	// 归一化目标角度
	while (TargetAngle > 180.0f) TargetAngle -= 360.0f;
	while (TargetAngle <= -180.0f) TargetAngle += 360.0f;
}

/* 电机总控制(定时中断调用) */
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
			
			if (fabs(pid_angle.Error[0]) < 3.0f) { // 可调整的阈值
				turn_done_cnt++;
			} 
			else {
				turn_done_cnt = 0;  // 如果误差变大，则重置计数器
			}
			
			if (turn_done_cnt >= 30) { 
				turn_done_cnt = 0;
				MotionState = STOP; // 转向完成先切换到STOP状态
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

