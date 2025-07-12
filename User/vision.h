#ifndef __VISION__H
#define __VISION__H

#include "headfile.h"

extern uint8_t visionBuf[10];
extern volatile uint8_t vision_rx_len;
extern volatile uint8_t NumNow[2];	// 0 : 左	1 : 右
extern uint16_t NumTimes[2][8];
extern uint8_t NumReal[2][2];	    // 01左二 : 00左一 : 10右一 : 11右二
extern volatile uint8_t NumUpdate;
extern volatile uint8_t FarNum[4];
extern volatile uint8_t FarFarNum[2];

void ClearNum(void);
void VisionProc(void);

#endif

