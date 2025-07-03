#include "pid.h"

#define MAX_PWM 999		// 输出电机PWM限幅

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


// **角度误差计算** (确保误差始终在 -180° ~ 180°)
static float Compute_Angle_Error(float target_angle, float current_angle) {
    float error = target_angle - current_angle;
    return fmod(error + 180.0f, 360.0f) - 180.0f;
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
	int error = get_turn_error();
	PID_Speed(speed + error, speed - error);
	// printf("%d\n",error);
}

/* 电机总控制(定时中断调用) */
void MotorRun(void) {
	static uint16_t cnt = 0;
	switch(State)  {
		case WAIT: {
			PID_Speed(0, 0);
		}
		break;
		case STOP: {
			PID_Speed(0, 0);
		}
		break;
		case TURN: {
			switch(Angle) {
				case LEFT: {
					PID_Angle(-90);
				}
				break;
				case RIGHT: {
					PID_Angle(90);
				}
				break;
				case LEFT_UP: {
					PID_Angle(-45);
				}
				break;
				case RIGHT_UP: {
					PID_Angle(45);
				}
				break;
				case REVERSE: {
					PID_Angle(180);
				}
				break;
				case FRONT: {
					PID_Angle(0);
				}
			}
			cnt++;
			if(cnt >= 100) {
				cnt = 0;
				Event = TurnDone;
			}
		}
		break;
		case TRACK: {
			if(Speed == SLOW) {
				PID_Turn(10);
			}
			else if(Speed == FAST) {
				PID_Turn(30);
			}
		}
	}
}

