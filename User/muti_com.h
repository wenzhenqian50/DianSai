#ifndef __MUTI_COM__H
#define __MUTI_COM__H

#include "stdarg.h"
#include "string.h"
#include "usart.h"
#include "stdio.h"
#include "main.h"
#include "headfile.h"

#define DEVICE_ID 0xA1
#define PACKAGE_HEAD 0xAA
#define PACKAGE_TAIL 0xBB
#define PACKAGE_SIZE 20
#define TIMEOUT 100
#define COM_NUM &huart3

// ����״̬��ö��
typedef enum {
    STATE_WAIT_HEADER,      // �ȴ�֡ͷ
    STATE_RECEIVE_ID,       // �����豸ID
    STATE_RECEIVE_LENGTH,   // �������ݳ���
    STATE_RECEIVE_DATA,     // ��������
    STATE_WAIT_TAIL         // �ȴ�֡β
} PacketParseState_t;

extern uint8_t g_rx_byte;
extern volatile uint8_t EnableReturnFlag;

int SendPackage(int num_args, ...);
void ParsePackageByte(uint8_t byte);
void PackageDataProc(void);

#endif

