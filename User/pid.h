#ifndef _PID_H
#define _PID_H

#include "main.h"
#include "tim.h"

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

float PID_Calculate(pid_t *pid,float Measure,  float Target);
void set_motor_left_speed(int target_pwm);
void set_motor_right_speed(int target_pwm);
void PID_Speed(int speed_l, int speed_r);

#endif

