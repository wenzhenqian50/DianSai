#include "fsm.h"

// /* --------------- 初始化 --------------- */
// volatile SystemState_t State = WAIT;
// volatile SystemNow_t Now = GOTO;
// TrackSpeed_t Speed = FAST;
// volatile TurnAngle_t Angle = LEFT;
// volatile Event_t Event = NONE;
// volatile FindMode_t Mode = M;

// SystemParam_t Param = {
//     .Num = 0,
//     .AngleRecord = LEFT,
//     .CrossTimes = 0,
// 	.Local = 0,
// };

// static uint8_t isKeyPoint = 0;		// 保证四个数字位置只识别一次
// static uint8_t delayValue = 8;


// static SystemState_t StateTrans(SystemState_t State) {
// 	static uint8_t cnt = 0;
	
//     if(Now == GOTO) {   // 送药状态机
//         switch(State) {
// 			case WAIT: {
// 				if(adcValue <= 10 && Event == GetNumber) {     // 识别到数字同时车上放入药品
//                     State = TRACK;
// 					Speed = FAST;
//                     Event = NONE;
// 					Param.Local = 1;
//                 }
// 			}
// 			break;
//             case TRACK: {		// 循迹送药
//                 switch(Event) {
//                     case Cross: {	// 检测到路口
// 						cnt++;
// 						if(cnt >= delayValue) {		// 延时响应(调整转弯位置)
// 							cnt = 0;
// 							Param.CrossTimes++;    // 更新经过的路口数
// 							Event = NONE;
// 							if(Param.CrossTimes == 1) {    // 第一个路口
// 								if(Param.Num == 1) {
// 									State = TURN;
// 									Angle = LEFT;
// 									Param.AngleRecord[0] = Angle;
// 									Param.Local = 2;
// 								}
// 								else if(Param.Num == 2) {	
// 									State = TURN;
// 									Angle = RIGHT;
// 									Param.AngleRecord[0] = Angle;
// 									Param.Local = 2;
// 								}
// 								else {
// 									Param.Local = 7;	// 没有匹配的数字
// 									delayValue = 20;		// 调整缓冲
// 								}
// 							}
// 							else if(Param.CrossTimes == 2) {	// 第二个路口
// 								if(Param.Num == NumReal[0][0]) {	
// 									State = TURN;
// 									Angle = LEFT;
// 									Param.AngleRecord[0] = Angle;
// 									Param.Local = 8;
// 								}
// 								else if(Param.Num == NumReal[1][0]) {
// 									State = TURN;
// 									Angle = RIGHT;
// 									Param.AngleRecord[0] = Angle;
// 									Param.Local = 8;
// 								}
// 								else {
// 									Param.Local = 13;	// 没有匹配的数字
// 									Speed = FAST;
// 									delayValue = 55;		// 调整缓冲
// 								}
								
// 							}
// 							else if(Param.CrossTimes == 3) {	// 第三个路口
// 								if(Param.Num == NumReal[0][0] || Param.Num == NumReal[0][1]) {
// 									State = TURN;
// 									Angle = LEFT;
// 									Param.AngleRecord[1] = Angle;
// 									Param.Local = 14;
// 								}
// 								else if(Param.Num == NumReal[1][0] || Param.Num == NumReal[1][1]) {
// 									State = TURN;
// 									Angle = RIGHT;
// 									Param.AngleRecord[1] = Angle;
// 									Param.Local = 14;
// 								}
// 								else {
// 									State = STOP;		// 全程没有找到匹配的数字(停车)
// 									printf("Not Found\n");
// 									delayValue = 15;
// 								}
// 							}
// 							else if(Param.CrossTimes == 4) {	// 第四个路口
// 								if(Param.Num == NumReal[0][0]) {
// 									State = TURN;
// 									if(Param.AngleRecord[1] == LEFT) {	// 设置方位
// 										Param.Local = 16;
// 										Angle = REVERSE;
// 									}
// 									else {
// 										Param.Local = 22;
// 										Angle = FRONT;
// 									}
// 									Param.AngleRecord[0] = Angle;
// 								}
// 								else if(Param.Num == NumReal[1][0]) {
// 									State = TURN;
// 									if(Param.AngleRecord[1] == LEFT) {
// 										Param.Local = 16;
// 										Angle = REVERSE;
// 									}
// 									else {
// 										Param.Local = 22;
// 										Angle = FRONT;
// 									}
// 									Param.AngleRecord[0] = Angle;
// 								}
// 								else {
// 									State = STOP;		// 没有找到匹配的数字(停车)
// 									printf("Not Found\n");
// 								}
// 							}
// 							ClearNum();     // 清除路口前记录的数字缓存
							
// 							printf("Cross:%d\tLocal:%d\n",Param.CrossTimes,Param.Local);
// 						}
// 				    }
//                     break;
//                     case GetNumber: {	// 循迹过程检测到数字
// 						if(Param.Local == 13 && isKeyPoint == 0) {
// 							Speed = SLOW;   // 减速
// 							cnt++;
// 							if(cnt >= 24) {
// 								cnt = 0;
// 								ClearNum();
// 								State = TURN;
// 								Angle = LEFT_UP;
// 								Mode = L;
// 								isKeyPoint = 1;
// 							}
// 						}
// 						else if(Param.Local == 7 || Param.Local == 15 || Param.Local == 21) {
// 							Speed = SLOW;
// 						}
//                     }
//                     break;
//                     case End: {
// 						switch(Param.Local) {
// 							case 3: {
// 								State = STOP;
// 								Param.Local = 4;
// 							}
// 							break;
// 							case 5: {
// 								State = STOP;
// 								Param.Local = 6;
// 							}
// 							break;
// 							case 9: {
// 								State = STOP;
// 								Param.Local = 10;
// 							}
// 							break;
// 							case 11: {
// 								State = STOP;
// 								Param.Local = 12;
// 							}
// 							break;
// 							case 17: {
// 								State = STOP;
// 								Param.Local = 18;
// 							}
// 							break;
// 							case 19: {
// 								State = STOP;
// 								Param.Local = 20;
// 							}
// 							break;
// 							case 23: {
// 								State = STOP;
// 								Param.Local = 24;
// 							}
// 							break;
// 							case 25: {
// 								State = STOP;
// 								Param.Local = 26;
// 							}
// 							break;
// 							default: {
// 								printf("Stop Error!\n");
// 							}
// 						}
//                     }
// 					break;
//                     default: {
//                         State = TRACK;
//                     }
//                 }
//             }
//             break;
//             case TURN: {
// 				if(Event == TurnDone) {
// 					if(Param.Local == 13) {
// 						cnt++;
// 						if(cnt >= 50) {
// 							cnt = 0;
// 							Event = NONE;
// 							printf("Mode:%d\n",Mode);
// 							switch(Mode) {
// 								case L: {
// 									Angle = RIGHT_UP;
// 									Mode = R;
// 								}
// 								break;
// 								case R: {
// 									Mode = M;
// 									Angle = FRONT;
// 								}
// 								break;
// 								case M: {
// 									State = TRACK;
// 									Speed = SLOW_PLUS;
// 								}
// 								break;
// 							}
// 						}
// 					}
// 					else {
// 						switch(Param.Local) {		// 更新位置
// 							case 2: {
// 								if(Param.AngleRecord[0] == LEFT) Param.Local = 3;
// 								else Param.Local = 5;
// 							}
// 							break;
// 							case 8: {
// 								if(Param.AngleRecord[0] == LEFT) Param.Local = 9;
// 								else Param.Local = 11;
// 							}
// 							break;
// 							case 14: {
// 								if(Param.AngleRecord[0] == LEFT) Param.Local = 15;
// 								else Param.Local = 21;
// 							}
// 							break;
// 							case 16: {
// 								if(Param.AngleRecord[0] == LEFT) Param.Local = 17;
// 								else Param.Local = 19;
// 							}
// 							break;
// 							case 22: {
// 								if(Param.AngleRecord[0] == LEFT) Param.Local = 23;
// 								else Param.Local = 25;
// 							}
// 						}
// 						State = TRACK;
// 						Speed = FAST;
// 						Event = NONE;
// 					}
// 				}
//             }
//             break;
//             case STOP: {
// 				if(adcValue >= 20) {	// 卸下药品,开始返回
// 					Now = RETURN;
// 					switch(Param.AngleRecord[0]) {
// 						case LEFT: {
// 							Angle = RIGHT;
// 						}
// 						break;
// 						case RIGHT: {
// 							Angle = LEFT;
// 						}
// 						break;
// 						case REVERSE: {
// 							Angle = FRONT;
// 						}
// 						break;
// 						case FRONT: {
// 							Angle = REVERSE;
// 						}
// 						break;
// 						default: {
// 							break;
// 						}
// 					}
// 					State = TURN;
// 				}
//             }
// 			break;
//         }
//     }
//     else if(Now == RETURN) {    // 返回状态机
// 		switch(State){
// 			case STOP: {
				
// 			}
// 			break;
// 			case TURN: {
// 				if(Event == TurnDone) {
// 					if(Param.Local == 2 ||Param.Local == 4 || Param.Local == 6 || Param.Local == 8 || Param.Local == 10 || Param.Local == 12 || Param.Local == 14 || Param.Local == 16 || Param.Local == 18 || Param.Local == 20 || Param.Local == 22 || Param.Local == 24 || Param.Local == 26) {
// 						Param.Local--;		// 更新位置
// 					}
// 					Event = NONE;
// 					State = TRACK;
// 					Speed = FAST;
// 				}
// 			}
// 			break;
// 			case TRACK: {
// 				if(Event == Cross) {
// 					cnt++;
// 					if(cnt >= 6) {
// 						// 转回主路
// 						if(Param.Local == 17 || Param.Local == 19) {
// 							State = TURN;
// 							Angle = RIGHT;
// 							Param.Local = 16;
// 						}
// 						else if(Param.Local == 23 || Param.Local == 25) {
// 							State = TURN;
// 							Angle = LEFT;
// 							Param.Local = 22;
// 						}
// 						else if(Param.Local == 15  || Param.Local == 21) {
// 							State = TURN;
// 							Angle = REVERSE;
// 							Param.Local = 14;
// 						}
// 						else if(Param.Local == 9  || Param.Local == 11) {
// 							State = TURN;
// 							Angle = REVERSE;
// 							Param.Local = 8;
// 						}
// 						else if(Param.Local == 3  || Param.Local == 5) {
// 							State = TURN;
// 							Angle = REVERSE;
// 							Param.Local = 2;
// 						}
						
// 						// 径直穿过路口
// 						if(Param.Local == 13) Param.Local = 7;
// 						else if(Param.Local == 7) Param.Local = 1;
						
// 						Event = NONE;
// 						Param.CrossTimes--;
// 					}
// 				}
// 				else if(Event == End) {
// 					if(Param.Local == 1) {
// 						State = STOP;
// 						Param.Local = 0;
// 					}
// 				}
// 			}
// 			break;
// 			default: {
// 				State = State;
// 			}
// 		}
//     }
// 	return State;	//  没有事件驱动，保持当前状态
// }

// void StateExecute(SystemState_t State) {
//     switch(State) {
//         case WAIT:	 // 等待
			
//         break;
//         case STOP:   // 停车

//         break;
//         case TRACK:  // 循迹

//         break;
//         case TURN:   // 转弯
				
//         break;
//     }
// }



