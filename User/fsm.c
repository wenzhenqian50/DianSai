#include "fsm.h"

/* ----- 初始化 ----- */
volatile SystemState_t State = WAIT;
volatile SystemNow_t Now = GOTO;
TrackSpeed_t Speed = FAST;
TurnAngle_t Angle = LEFT;
volatile Event_t Event = NONE;
volatile FindMode_t Mode = M;
SystemParam_t Param = {
    .Num = 0,
    .AngleRecord = LEFT,
    .CrossTimes = 0,
};


SystemState_t StateTrans(SystemState_t State, Event_t Event) {
    if(Now == GOTO) {   // 送药状态机
        switch(State) {
			case WAIT: {
				if(adcValue <= 10 || Event == GetNumber) {     // 识别到数字同时车上放入药品
                    State = TRACK;
                    Event = NONE;
                }
			}
			break;
            case TRACK: {
                switch(Event) {
                    case Cross: {
                        if(Param.CrossTimes == 0) {    // 前两个路口
                            if(Param.Num == 1) {
                                State = TURN;
                                Angle = LEFT;
                                Param.AngleRecord[0] = Angle;
                            }
                            else if(Param.Num == 0) {
                                State = TURN;
                                Angle = RIGHT;
                                Param.AngleRecord[0] = Angle;
                            }
                        }
                        else if(Param.CrossTimes == 1) {
                            if(Param.Num == NumReal[0][0]) {
                                State = TURN;
                                Angle = LEFT;
                                Param.AngleRecord[0] = Angle;
                            }
                            else if(Param.Num == NumReal[1][0]) {
                                State = TURN;
                                Angle = RIGHT;
                                Param.AngleRecord[0] = Angle;
                            }
                        }
                        else if(Param.CrossTimes == 2) {
                            if(Param.Num == NumReal[0][0] || Param.Num == NumReal[0][1]) {
                                State = TURN;
                                Angle = LEFT;
                                Param.AngleRecord[0] = Angle;
                            }
                            else if(Param.Num == NumReal[1][0] || Param.Num == NumReal[1][1]) {
                                State = TURN;
                                Angle = RIGHT;
                                Param.AngleRecord[0] = Angle;
                            } 
							else {
								
							}
                            
                        }
                        ClearNum();     // 清除路口前的数字缓存
                        Param.CrossTimes++;
                    }
                    break;
                    case GetNumber: {
                        Speed = SLOW;   // 循迹过程检测到数字就减速
                    }
                    break;
                    case End: {
                        State = STOP;
                    }
                    default: {
                        State = TRACK;
                    }
                }
            }
            break;
            case TURN: {
				if(Event == TurnDone) {
					State = TRACK;
					Speed = FAST;
				}
            }
            break;
            case STOP: {

            }
        }
    }
    else if(Now == RETURN) {    // 返回状态机

    }
   return State;	//  没有事件驱动，保持当前状态
}

void StateExecute(SystemState_t State) {
    switch(State) {
        case WAIT:	 // 等待

        break;
        case STOP:   // 停车

        break;
        case TRACK:  // 循迹

        break;
        case TURN:   // 转弯
				
        break;
    }
}

void SystemRun(void) {
   State = StateTrans(State,Event);	        //更新状态
   StateExecute(State);	                    //执行状态任务函数
}