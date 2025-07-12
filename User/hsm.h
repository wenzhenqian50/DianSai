#ifndef __HSM__H
#define __HSM__H

#include "headfile.h"

#define QUEUE_SIZE 8   // 事件队列容量

/* 系统事件枚举 */
typedef enum {
    NONE,           // 无事件
    GetNum,         // 检测到数字
    TurnDone,       // 转向完成
    PassCross,      // 通过十字路口
    PassCross_T,    // 通过T字路口
    ArriveEnd,      // 到达末端终点
    GetDrug,        // 检测到药品
    LoseDrug,       // 药品被拿走
	MoveDone,		// 行进完成
	TimeOut,	    // 软件定时器超时事件
    GetCarData,     // 收到车二数据
}Event;

/* 小车运动状态枚举 */
typedef enum {
    TRACK,      // 循迹
    STOP,       // 停止
    TURN,       // 转向
	MOVE,		// 行进
}MotionState_t;

/* 循迹速度枚举 */
typedef enum {
    SLOW,       // 慢速
    MID,        // 中速
    FAST,       // 快速
}TrackSpeed_t;

/* 视觉模块数据处理模式枚举 */
typedef enum {
    L,          
    M,          
    R,          
}FindMode_t;

/* 任务阶段枚举 */
typedef enum { 
    PHASE_IDLE,
    PHASE_FORWARD, 
    PHASE_RETURN,
} MissionPhase;

/* 事件结构体 */
typedef struct {
    Event type;   // 事件类型
}Event_t;

/* 系统参数结构体 */
typedef struct SystemParam_t {
    uint8_t CrossTimes;     // 经过的路口数
    uint8_t Num;            // 运送的病房号
    uint8_t Local;          // 当前位置
}SystemParam_t;

/* 事件队列结构体 */
typedef struct {
    Event buffer[QUEUE_SIZE];
    volatile int head;       // 读事件位置
    volatile int tail;       // 写事件位置
    volatile int count;      // 事件总数
}EventQueue_t;

/* ---------------全局变量定义--------------- */
extern volatile EventQueue_t EventQueue;
extern volatile FindMode_t FindMode;
extern volatile SystemParam_t SysParam;
extern volatile MotionState_t MotionState;
extern volatile TrackSpeed_t TrackSpeed;
/* ----------------------------------------- */

/* 声明状态函数统一原型 */
typedef bool (*StateHandler)(Event_t* event);

/* 外部API */
void HsmInit(void);                // 初始化状态机
void HsmDispatch(void);            // 分发事件给状态机
void QueueInit(volatile EventQueue_t* q);
bool queue_pop(volatile EventQueue_t* q, Event* event);
bool queue_push(volatile EventQueue_t* q, Event event);

#endif


