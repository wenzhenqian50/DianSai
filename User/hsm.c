#include "hsm.h"

/* --------------------- 定义所有变量 --------------------- */
volatile EventQueue_t EventQueue;
volatile MotionState_t MotionState;
volatile TrackSpeed_t TrackSpeed;
volatile SystemParam_t SysParam;
volatile FindMode_t FindMode;

// 全局状态指针
static MissionPhase mission_phase;
static StateHandler current_state;      

/* -------------------- 声明所有状态函数 -------------------- */

/* 顶层状态 */
static bool IdleState(Event_t* event);
static bool ForwardMission(Event_t* event);
static bool ReturnMission(Event_t* event);

/* 中间状态 */
static bool Driving(Event_t* event);

/* 送药底层状态 */
static bool TrackState_f(Event_t* event);
static bool StopState_f(Event_t* event);
static bool TurnState_f(Event_t* event);
static bool MoveState_f(Event_t* event);

/* 放回底层状态 */
static bool TrackState_r(Event_t* event);
static bool StopState_r(Event_t* event);
static bool TurnState_r(Event_t* event);
static bool MoveState_r(Event_t* event);


/* ------------------------------ 事件队列 ------------------------------ */

/* 初始化队列 */
void QueueInit(volatile EventQueue_t* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

/* 队列判满 */
static bool queue_is_full(const volatile EventQueue_t* q) {
    return q->count == QUEUE_SIZE;
}

/* 队列判空 */
static bool queue_is_empty(const volatile EventQueue_t* q) {
    return q->count == 0;
}

/* 事件入队(生产者调用) */
bool queue_push(volatile EventQueue_t* q, Event event) {
    __disable_irq(); 		 // 关闭全局中断
    
    if (queue_is_full(q)) {	 // 队列已满
        __enable_irq(); 	 // 恢复中断
        return false; 
    }
    
    q->buffer[q->tail] = event;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;
    
    __enable_irq(); 		 // 打开全局中断
    return true;
}

/* 事件出队(消费者调用) */
bool queue_pop(volatile EventQueue_t* q, Event* event) {
    __disable_irq(); 		 // 关闭全局中断
    
    if (queue_is_empty(q)) { // 队列为空
        __enable_irq(); 	 // 恢复中断
        return false;
    }
    
    *event = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;
    
    __enable_irq(); 		 // 打开全局中断
    return true;
}

/* --------------------------------------------------------------------- */


/* 状态转换函数 */
static void TransitionTo(StateHandler next_state) {
    // 执行 Exit 动作
    // if(current_state == IdleState) { 
        
    // }
    // else if(current_state == ForwardMission) {
        
    // }
    // else if(current_state == ReturnMission) {
        
    // } 
    // else if(current_state == TrackState_f) { 
        
    // } 
    // else if(current_state == StopState_f) {
        
    // }
    // else if(current_state == TurnState_f) { 
        
    // }
    // else if(current_state == TrackState_r) { 
        
    // } 
    // else if(current_state == StopState_r) { 
        
    // }
    // else if(current_state == TurnState_r) { 
         
    // }
    
    current_state = next_state;     // 状态切换
    
    // 执行 Entry 动作
    if(current_state == IdleState) { 
        MotionState = STOP; 
    }
    else if(current_state == ForwardMission) {
        MotionState = STOP;
    }
    else if(current_state == ReturnMission) {
        MotionState = STOP;
    } 
    else if(current_state == TrackState_f) { 
        MotionState = TRACK;  
    } 
    else if(current_state == StopState_f) {
        MotionState = STOP; 
    }
    else if(current_state == TurnState_f) { 
        MotionState = TURN;
    }
    else if(current_state == MoveState_f) {
        MotionState = MOVE;
    }
    else if(current_state == TrackState_r) { 
        MotionState = TRACK; 
    } 
    else if(current_state == StopState_r) { 
        MotionState = STOP; 
    }
    else if(current_state == TurnState_r) { 
        MotionState = TURN;
    }
    else if(current_state == MoveState_r) {
        MotionState = MOVE;
    }
}


/* -------------------- 状态函数的实现 -------------------- */

/**
 * @brief 顶层状态 1: 空闲状态
 */
static bool IdleState(Event_t* event) {
    if (event->type == GetDrug) {
        SysParam.Local = 1;                // 更新位置
        mission_phase = PHASE_FORWARD;     // 切换为送程任务
        TransitionTo(TrackState_f);        // 循迹状态
        return true;
    }
    return false;  // 其他状态直接返回
}

/**
 * @brief 顶层状态 2: 去程任务
 */
static bool ForwardMission(Event_t* event) {
    switch(event->type) {
        case LoseDrug:                      // 等待药品被取走
            SetTurnAngle(190);              // 掉头
            mission_phase = PHASE_RETURN;   // 切换到返程任务
            TransitionTo(TurnState_r);
			if(SysParam.Local == 18 || SysParam.Local == 20 || SysParam.Local == 24 || SysParam.Local == 26) {
                if(FarFarNum[0] == FarNum[0] || FarFarNum[0] == FarNum[1] || FarFarNum[1] == FarNum[0] || FarFarNum[1] == FarNum[1]) {
                    SendPackage(5,3,FarNum[0],FarNum[1],FarNum[2],FarNum[3]); // 告诉车二远端病房路口信息
                }
                else if(FarFarNum[0] == FarNum[2] || FarFarNum[0] == FarNum[3] || FarFarNum[1] == FarNum[2] || FarFarNum[1] == FarNum[3]) {
                    SendPackage(5,4,FarNum[0],FarNum[1],FarNum[2],FarNum[3]); // 告诉车二远端病房路口信息
                }
            }
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);	    // 熄灭红灯
            return true;
        default:
            return false;
    }
}

/**
 * @brief 顶层状态 3: 返程任务
 */
static bool ReturnMission(Event_t* event) {
    switch(event->type) {
        case ArriveEnd:
            if(SysParam.Local == 1) {
                if(SysTimer.timeout) {  // 必须达到超时时间
                    SysParam.Local = 0;
                    mission_phase = PHASE_IDLE;
                    TransitionTo(IdleState);    							// 任务完成，回到空闲状态
				    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);		// 点亮绿灯
                }
            }
            return true;
        default:
            return false;
    }
}

/*  送药和返程共用的程式 */
static bool Driving(Event_t* event) {
    if(mission_phase == PHASE_FORWARD) {
        return ForwardMission(event);
    }
    else if(mission_phase == PHASE_RETURN) {
        return ReturnMission(event);
    }
    return false; 
}


/* ---------- 底层状态函数 ----------*/

static bool TrackState_f(Event_t* event) {

    switch (event->type) {
		case PassCross_T:
        case PassCross:
            if(SysParam.Local == 1) {
                if(SysParam.Num == 1 || SysParam.Num == 2) {
                    SysParam.Local = 2;
                    SetDistanceParam(600);
                    TransitionTo(MoveState_f);
                }
                else {
                    SysParam.Local = 7;     // 更新位置
                }
            }
            else if(SysParam.Local == 7) {
                if(SysParam.Num == NumReal[0][0] || SysParam.Num == NumReal[1][0]) {
                    SysParam.Local = 8;
                    SetDistanceParam(600);
                    TransitionTo(MoveState_f);
                }
                else {
                    HAL_UART_Transmit(&huart6,(uint8_t *)"1",1,100);     // 将视觉模块切换为双数字远端病房识别模式
                    SoftTimer_Start(&SysTimer,2300);                     // 经验值(根据到四个病房号处的距离估算)
                    SysParam.Local = 13;                                 // 继续向前
                }
            }   
            else if(SysParam.Local == 13) {
                if(SysParam.Num == NumReal[0][0] || SysParam.Num == NumReal[0][1] || SysParam.Num == NumReal[1][0] || SysParam.Num == NumReal[1][1]) {
					printf("%d\t%d,%d\t%d\n",NumReal[0][1],NumReal[0][0],NumReal[1][0],NumReal[1][1]);
                    FarNum[0] = NumReal[0][1];  // 将路口信息放入缓存
                    FarNum[1] = NumReal[0][0];
                    FarNum[2] = NumReal[1][0];
                    FarNum[3] = NumReal[1][1];
                    SetDistanceParam(700);      // 找到匹配数字,切换距离环
                    TransitionTo(MoveState_f);
                }
                else {                          // 没有找到匹配的数字
                    TransitionTo(StopState_f);  // 原地停车
                }
                SysParam.Local = 14;            // 更新位置
            } 
            else if(SysParam.Local == 15) {
                if(SysParam.Num == NumReal[0][0] || SysParam.Num == NumReal[1][0]) {
                    FarFarNum[0] = NumReal[0][0];
                    FarFarNum[1] = NumReal[1][0];
                    SetDistanceParam(600);
                    TransitionTo(MoveState_f);
                }
                else {                          // 没有找到匹配的数字
                    TransitionTo(StopState_f);  // 原地停车
                }
                SysParam.Local = 16;            // 更新位置
            }   
            else  if(SysParam.Local == 21) {
                if(SysParam.Num == NumReal[0][0] || SysParam.Num == NumReal[1][0]) {
                    FarFarNum[0] = NumReal[0][0];
                    FarFarNum[1] = NumReal[1][0];
                    SetDistanceParam(600);
                    TransitionTo(MoveState_f);
                }
                else {                          // 没有找到匹配的数字
                    TransitionTo(StopState_f);  // 原地停车
                }
                SysParam.Local = 22;            // 更新位置
            }
            ClearNum();     // 每次经过路口都要对数字缓冲进行清理 
            return true;
        case GetNum:
            if(SysParam.Local == 13) {
                
            }
            return true;
        case ArriveEnd:
            if(SysParam.Local == 3 || SysParam.Local == 5 || SysParam.Local == 9 || SysParam.Local == 11 || SysParam.Local == 17 || SysParam.Local == 19 || SysParam.Local == 23 || SysParam.Local == 25) {
                if(SysTimer.timeout == 1) {
                    SysParam.Local += 1;
                    if(SysParam.Local == 12) {      
                        SendPackage(1,1);   // 告诉车二转向方式
                    }
                    else if(SysParam.Local == 10) {
                        SendPackage(1,2);   // 告诉车二转向方式
                    }
                    TransitionTo(ForwardMission);							// 到达病房,等待取药
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);		// 点亮红灯
                }  
            }
            return true;
        case TimeOut:
            if(SysParam.Local == 13) {
                SysTimer.timeout = 0;   // 清除软件定时器超时标志
                SetTurnAngle(-35);      // 转到左上方
                TransitionTo(TurnState_f);
            }
			return true;
        default:
            return false;
            // return Driving(event);  // 没有匹配状态调用父函数
    }
}

static bool StopState_f(Event_t* event) {

    switch (event->type) {
        case PassCross:

            return true;
        case PassCross_T:

            return true;
        case GetNum:

            return true;
        default:
            return false;
            // return Driving(event);  // 没有匹配状态调用父函数
    }
}


static bool TurnState_f(Event_t* event) {
    static uint8_t DirFlag = 0;     // 0:左上 1:中间 2:右上
    switch (event->type) {
        case TurnDone:
            if(SysParam.Local == 2) {
                if(SysParam.Num == 1) {
                    SysParam.Local = 3;
                }
                else if(SysParam.Num == 2) {
                    SysParam.Local = 5;
                }
                TransitionTo(TrackState_f);
                SoftTimer_Start(&SysTimer,500);     // 启动一个定时器, 防止受数字影响识别为终点
            }
            else if(SysParam.Local == 8) {
                if(SysParam.Num == NumReal[0][0]) {
                    SysParam.Local = 9;
                }
                else if( SysParam.Num == NumReal[1][0]) {
                    SysParam.Local = 11;
                }
                TransitionTo(TrackState_f);
                SoftTimer_Start(&SysTimer,500);     // 启动一个定时器, 防止受数字影响识别为终点
            }
            else if(SysParam.Local == 14) {
                if(SysParam.Num == NumReal[0][0] || SysParam.Num == NumReal[0][1]) {
                    SysParam.Local = 15;
                }
                else if(SysParam.Num == NumReal[1][0] || SysParam.Num == NumReal[1][1]) {
                    SysParam.Local = 21;
                }
                TransitionTo(TrackState_f);
                SoftTimer_Start(&SysTimer,500);     // 启动一个定时器, 防止受数字影响识别为终点
            }
            else if(SysParam.Local == 16) {
                if(SysParam.Num == NumReal[0][0]) {
                    SysParam.Local = 17;
                }
                else if(SysParam.Num == NumReal[1][0]) {
                    SysParam.Local = 19;
                }
                TransitionTo(TrackState_f);
                SoftTimer_Start(&SysTimer,500);     // 启动一个定时器, 防止受数字影响识别为终点
            }
            else if(SysParam.Local == 22) {
                if(SysParam.Num == NumReal[0][0]) {
                    SysParam.Local = 23;
                }
                else if(SysParam.Num == NumReal[1][0]) {
                    SysParam.Local = 25;
                }
                TransitionTo(TrackState_f);
                SoftTimer_Start(&SysTimer,500);     // 启动一个定时器, 防止受数字影响识别为终点
            }
            else if(SysParam.Local == 13)   {           // 识别四病房号状态
                if(DirFlag == 0) {
                    FindMode = L;
					ClearNum();
                    SoftTimer_Start(&SysTimer, 1000);   // 待左边数字识别稳定
                }
                else if(DirFlag == 1) {
                    FindMode = M;
					ClearNum();
                    SoftTimer_Start(&SysTimer, 1000);   // 待中间数字稳定
                }
                else if(DirFlag == 2) {
					ClearNum();
                    FindMode = R;
                    SoftTimer_Start(&SysTimer, 1000);   // 待右边数字识别稳定
                }
            }
			return true;
        case TimeOut:
            if(SysParam.Local == 13) {
                if(DirFlag == 0) {
                    DirFlag = 2;
                    SysTimer.timeout = 0;       // 清除超时标志
                    SetTurnAngle(70);           // 转到右上方
					TransitionTo(TurnState_f);
                }
                else if(DirFlag == 1) {

                    DirFlag = 0;                // 恢复初始值
                    SysTimer.timeout = 0;
                    TransitionTo(TrackState_f); // 切换为循迹状态
                }
                else if(DirFlag == 2) {
                    DirFlag = 1;
                    SysTimer.timeout = 0;
                    SetTurnAngle(-35);          // 转到中间主道
					TransitionTo(TurnState_f);
                }
            }
            return true;
        default:
            return false;
            // return Driving(event);           // 没有匹配状态调用父函数
    }
}

static bool MoveState_f(Event_t* event) {

    switch (event->type) {
        case MoveDone:
            if(SysParam.Local == 2) {
                if(SysParam.Num == 1) {
                    SetTurnAngle(-90);  // 左转
                    TransitionTo(TurnState_f);
                }
                else if(SysParam.Num == 2) {
                    SetTurnAngle(90);   // 右转
                    TransitionTo(TurnState_f);
                }
            }
            else if(SysParam.Local == 8) {
                if(SysParam.Num == NumReal[0][0]) {
                    SetTurnAngle(-90);  // 左转
                    TransitionTo(TurnState_f);
                }
                else if(SysParam.Num == NumReal[1][0]) {
                    SetTurnAngle(90);   // 右转
                    TransitionTo(TurnState_f);
                }
            }
            else if(SysParam.Local == 14) {
                if(SysParam.Num == NumReal[0][0] || SysParam.Num == NumReal[0][1]) {
                    SetTurnAngle(-90);  // 左转
                    TransitionTo(TurnState_f);
                }
                else if(SysParam.Num == NumReal[1][0] || SysParam.Num == NumReal[1][1]) {
                    SetTurnAngle(90);   // 右转
                    TransitionTo(TurnState_f);
                }
            }
            else if(SysParam.Local == 16) {
                if(SysParam.Num == NumReal[0][0]) {
                    SetTurnAngle(-90);  // 左转
                    TransitionTo(TurnState_f);
                }
                else if(SysParam.Num == NumReal[1][0]) {
                    SetTurnAngle(90);   // 右转
                    TransitionTo(TurnState_f);
                }
            }
            else if(SysParam.Local == 22) {
                if(SysParam.Num == NumReal[0][0]) {
                    SetTurnAngle(-90);  // 左转
                    TransitionTo(TurnState_f);
                }
                else if(SysParam.Num == NumReal[1][0]) {
                    SetTurnAngle(90);   // 右转
                    TransitionTo(TurnState_f);
                }
            }
            return true;
        default:
            return false;
            // return Driving(event);  // 没有匹配状态调用父函数
    }
}



static bool TrackState_r(Event_t* event) {

    switch (event->type) {
        case PassCross_T:
        case PassCross:
            if(SysParam.Local == 3 || SysParam.Local == 5) {
                SetDistanceParam(600);
                TransitionTo(MoveState_r);
            }
            else if(SysParam.Local == 9 || SysParam.Local == 11) {
                SetDistanceParam(600);
                TransitionTo(MoveState_r);
            }
            else if(SysParam.Local == 13) {
                SysParam.Local = 7;                 // 直接穿过第二个十字路口
            }
            else if(SysParam.Local == 7) {
                SysParam.Local = 1;                 // 直接穿过第一个十字路口
                SoftTimer_Start(&SysTimer,400);     // 启动一个定时器,防止干扰
            }
            else if(SysParam.Local == 15 || SysParam.Local == 21) {     // 理论上是T字路口
                SetDistanceParam(600);
                TransitionTo(MoveState_r);
            }
            else if(SysParam.Local == 17 || SysParam.Local == 19) {
                SetDistanceParam(600);
                TransitionTo(MoveState_r);
            }
            else if(SysParam.Local == 23 || SysParam.Local == 25) {
                SetDistanceParam(600);
                TransitionTo(MoveState_r);
            }
            return true;
        default:
            return Driving(event);  // 没有匹配状态调用父函数(冒泡)
    }
}



static bool StopState_r(Event_t* event) {
    switch(event->type) {
        case GetCarData:                    // 等待车二事件
            if (EnableReturnFlag == 1)      // 接收到启动指令
            {
                SysParam.Local -= 1;
                TransitionTo(TrackState_r); // 开始循迹返回
            }
            return true;
        default:
            return false;
            // return Driving(event);  // 没有匹配状态调用父函数
    }
}


static bool TurnState_r(Event_t* event) {

    switch (event->type) {
        case TurnDone:
            if(SysParam.Local == 4 || SysParam.Local == 6 || SysParam.Local == 10 || SysParam.Local == 12) {             // 近端和中端病房返回
                SysParam.Local -= 1;
                TransitionTo(TrackState_r);     // 开始循迹返回
            }
            else if(SysParam.Local == 18 || SysParam.Local == 20 || SysParam.Local == 24 || SysParam.Local == 26) {             // 远端病房返回
                if(EnableReturnFlag == 1) {     // 常规返回
                    SysParam.Local -= 1;
                    TransitionTo(TrackState_r); // 正常循迹返回
                }
                else {
                    TransitionTo(StopState_r);  // 切换停止状态
                }
            }
            else if(SysParam.Local == 2 || SysParam.Local == 8 || SysParam.Local == 14 || SysParam.Local == 16 || SysParam.Local == 22) {
                SoftTimer_Start(&SysTimer,400); // 启动一个定时器,防止干扰
                SysParam.Local -= 1;    
                TransitionTo(TrackState_r);     // 返回途经过第一个路口
            }
            return true;
        default:
            return false;
            // return Driving(event);  // 没有匹配状态调用父函数
    }
}

static bool MoveState_r(Event_t* event) {

    switch (event->type) {
        case MoveDone:  // 根据当前位置进行转向控制
            if(SysParam.Local == 3) {
                SysParam.Local = 2;
                SetTurnAngle(90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 5) {
                SysParam.Local = 2;
                SetTurnAngle(-90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 9) {
                SysParam.Local = 8;
                SendPackage(1,0);   // 告诉车二可以开始动身
                SetTurnAngle(90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 11) {
                SysParam.Local = 8;
                SendPackage(1,0);   // 告诉车二可以开始动身
                SetTurnAngle(-90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 15) {
                SysParam.Local = 14;
                SetTurnAngle(90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 21) {
                SysParam.Local = 14;
                SetTurnAngle(-90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 17) {
                SysParam.Local = 16;
                SetTurnAngle(90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 19) {
                SysParam.Local = 16;
                SetTurnAngle(-90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 23) {
                SysParam.Local = 22;
                SetTurnAngle(90);
                TransitionTo(TurnState_r);
            }
            else if(SysParam.Local == 25) {
                SysParam.Local = 22;
                SetTurnAngle(-90);
                TransitionTo(TurnState_r);
            }
            return true;
        default:
            return false;
            // return Driving(event);  // 没有匹配状态调用父函数
    }
}



/* -------------------- 外部API实现 -------------------- */

/* HSM初始化 */
void HsmInit() {
    mission_phase = PHASE_IDLE;		// 初始空闲任务
    current_state = IdleState;      // 空闲状态

    MotionState = STOP;		        // 电机停止
    TrackSpeed = FAST;              // 循迹快速模式
    SysParam.CrossTimes = 0;        // 通过路口数为零
    SysParam.Local = 0;             // 起始位置为零
    SysParam.Num = 0;               // 目标病房号为零(无效)
    FindMode = M;                   // 识别数字模式(中部左右识别)
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 & GPIO_PIN_1, GPIO_PIN_SET);	// 置位红绿灯
    /* 其他初始化程式 */
}

/* 状态分配 */
void HsmDispatch(void) {
    Event_t event_t;
    Event event = NONE;
    queue_pop(&EventQueue,&event);  // 从队列获取一个事件
    event_t.type = event;
    current_state(&event_t);        // 运行状态函数
}



