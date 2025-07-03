#ifndef __FSM__H
#define __FSM__H

#include "sensor.h"
#include "main.h"
#include "menu.h"
#include "lcd.h"
#include "pid.h"

typedef enum {
    GOTO,
    RETURN,
}SystemNow_t;

typedef enum {
    WAIT,
    TRACK,
    STOP,
    TURN,
}SystemState_t;

typedef enum {
    GetNumber,
    LoseDrug,
    TurnDone,
    GetDrug,
    Cross,
    End,
    NONE,
}Event_t;

typedef enum {
    LEFT,
    LEFT_UP,
    RIGHT,
    RIGHT_UP,
    REVERSE,
    FRONT,
}TurnAngle_t;

typedef enum {
    SLOW,
    FAST,
}TrackSpeed_t;

typedef enum {
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
}Number_t;

typedef enum {
    L,
    M,
    R,
}FindMode_t;

typedef struct SystemParam_t {
    TurnAngle_t AngleRecord[2];
    uint8_t CrossTimes;
    uint8_t Num;
}SystemParam_t;

extern volatile SystemState_t State;
extern volatile SystemNow_t Now;
extern TrackSpeed_t Speed;
extern TurnAngle_t Angle;
extern volatile Event_t Event;
extern volatile FindMode_t Mode;
extern SystemParam_t Param;


void SystemRun(void);

#endif


