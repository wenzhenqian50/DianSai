#ifndef _PID_H
#define _PID_H

#include "headfile.h"

/* PID�ṹ�岿�� */
/* PID����Ȩ�� */
/* Ŀ��ֵ����ȡֵ��������� */
/* ���������֡�΢������������ */
/* ����޷��Լ������޷� */	
typedef struct {
    float Kp;              // ����ϵ��
    float Ki;              // ����ϵ��
    float Kd;              // ΢��ϵ��
    float Target;          // Ŀ��ֵ
    float Measure;         // ����ֵ
    float Error[3];        // ������� [��ǰ���, ��һ�����, ���ϴ����]
    float KpOut;           // �������
    float KiOut;           // �������
    float KdOut;           // ΢�����
    float PID_Out;         // PID�������
    float PID_Limit_MAX;   // PID����޷�
    float Ki_Limit_MAX;    // �����޷�
} pid_t;

extern pid_t pid_l;
extern pid_t pid_r;
extern pid_t pid_d;
extern pid_t pid_angle;
extern pid_t pid_turn;

extern volatile float TargetAngle;
extern volatile int CountAll;
extern volatile float Angle_d;
extern volatile int Length;

void set_motor_left_speed(int target_pwm);
void set_motor_right_speed(int target_pwm);
void PID_Speed(int speed_l, int speed_r);
void PID_Angle(float angle);
void PID_Turn(int speed);
void PID_Distance(void);
void MotorRun(void);
void SetTurnAngle(float angle);
void SetDistanceParam(int length);

#endif

