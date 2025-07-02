#include "fsm.h"

SystemState_t State = WAIT;
SystemNow_t Now = GOTO;
Direction_t Dir;
Event_t Event;


//SystemState_t StateTrans(SystemState_t State, Event_t Event) {
//    switch(State) {
//        case WAIT:{
//            switch (Event) {
//                case GetNumber: {
//                    State = STOP;
//                }
//                default: {
//                    State = WAIT;
//                }
//            }
//        }
//        break;
//        case TRACK: {
//            switch(Event) {
//                case Cross: {
//                    State = TURN;
//                }
//                default: {
//                    State = TRACK;
//                }
//            }
//        }
//        break;
//        case TURN: {
//            switch(Event) {
//                case 
//            }
//        }
//        break;
//        case STOP: {

//        }
//    }
//    return State;	//没有事件驱动，保持当前状态
//}

//void StateExecute(SystemState_t state) {
//    switch(state) {
//        case STATE_IDLE:
//            //执行空闲状态任务
//            break;
//        case STATE_RUNNING:
//            //执行运行状态任务
//            break;
//        case STATE_ERROR:
//            //执行错误状态任务
//            break;
//    }
//}

//void SystemRun(void) {
//    // Event_t event  = GetEvent();	            //获取当前事件
//    State = StateTrans(State,Event);	        //更新状态
//    StateExecute(State);	                    //执行状态任务函数
//}