#ifndef _PID_H
#define _PID_H

#include "main.h"
#include "tim.h"

/* PID结构体部分 */
/* PID三个权重 */
/* 目标值、获取值、三个误差 */
/* 比例、积分、微分输出、总输出 */
/* 输出限幅以及积分限幅 */	
typedef struct {
    float Kp;              // 比例系数
    float Ki;              // 积分系数
    float Kd;              // 微分系数
    float Target;          // 目标值
    float Measure;         // 测量值
    float Error[3];        // 误差数组 [当前误差, 上一次误差, 上上次误差]
    float KpOut;           // 比例输出
    float KiOut;           // 积分输出
    float KdOut;           // 微分输出
    float PID_Out;         // PID最终输出
    float PID_Limit_MAX;   // PID输出限幅
    float Ki_Limit_MAX;    // 积分限幅
} pid_t;

extern pid_t pid_l;
extern pid_t pid_r;

float PID_Calculate(pid_t *pid,float Measure,  float Target);
void set_motor_left_speed(int target_pwm);
void set_motor_right_speed(int target_pwm);
void PID_Speed(int speed_l, int speed_r);

#endif

