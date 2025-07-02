#ifndef __FSM__H
#define __FSM__H

#include "main.h"
#include "menu.h"
#include "lcd.h"

typedef enum {
    GOTO,
    RETURN,
}SystemNow_t;

typedef enum {
    TRACK,
    WAIT,
    STOP,
    TURN,
}SystemState_t;

typedef enum {
    GetDrug,
    LoseDrug,
    GetNumber,
    TurnDone,
    Range_s,
    Range_m, 
    Range_l,
    Cross,
}Event_t;

typedef enum {
    LEFT,
    RIGHT,
    REVERSE,
}Direction_t;

typedef enum {
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
}Number_t;

extern SystemState_t State;
extern SystemNow_t Now;
extern Direction_t Dir;
extern Event_t Event;

void SystemRun(void);

#endif


