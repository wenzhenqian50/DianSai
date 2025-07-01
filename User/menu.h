#ifndef __MENU_H__
#define __MENU_H__

#include <stddef.h>
#include <stdint.h>
#include "stdlib.h"
#include <stdio.h>
#include "main.h"
#include "lcd.h"

// --- 结构体定义 ---

// 前置声明
struct Menu_t; 
struct MenuItem_t;

// 菜单项结构体
typedef struct MenuItem_t {
    const char* text;                   // 菜单项显示的文本
    
    // 菜单项的子菜单或动作 (二选一)
    struct MenuItem_t* subMenu;         // 指向子菜单项数组的第一个元素
    size_t subMenuSize;                 // 子菜单项的数量
    void (*actionCallback)(void);       // 动作回调函数

    // --- 以下为框架内部使用，用户无需关心 ---
    struct MenuItem_t* next;            // 指向下一个兄弟节点
    struct MenuItem_t* prev;            // 指向前一个兄弟节点
    struct MenuItem_t* parent;          // 指向父菜单项
} MenuItem_t;

// 菜单系统 "对象"
typedef struct Menu_t {
    MenuItem_t* rootMenu;               // 菜单树的根
    MenuItem_t* currentMenu;            // 当前可见菜单的第一个项
    MenuItem_t* currentSelection;       // 当前高亮选中的项
    MenuItem_t* topVisibleItem;         // 屏幕上显示的第一项（用于滚动）

    uint8_t needsRedraw;                // 刷新标志位
} Menu_t;

// 用户输入类型枚举
typedef enum {
    MENU_INPUT_NEXT,
    MENU_INPUT_PREVIOUS,
    MENU_INPUT_SELECT,
    MENU_INPUT_BACK
} MenuInput_e;


// 全局变量
extern Menu_t menu;

// --- 公共 API ---

/**
 * @brief 初始化菜单系统
 * @param menu 指向要初始化的菜单对象
 * @param rootItems 指向菜单项定义的根数组
 * @param rootItemCount 根数组中项的数量
 * @note 此函数会自动链接所有菜单项的 next, prev, parent 指针
 */
void Menu_Create(Menu_t* menu, MenuItem_t* rootItems, size_t rootItemCount);

/**
 * @brief 处理用户输入
 * @param menu 指向菜单对象
 * @param input 用户的输入类型 (NEXT, PREVIOUS, SELECT, BACK)
 */
void Menu_HandleInput(Menu_t* menu, MenuInput_e input);

/**
 * @brief 绘制菜单界面
 * @param menu 指向菜单对象
 * @note 内部会检查 needsRedraw 标志，只在需要时重绘
 */
void Menu_Draw(Menu_t* menu,lcd *plcd);

/**
 * @brief 请求下一次循环强制重绘菜单
 * @param menu 指向菜单对象
 */
void Menu_RequestRedraw(Menu_t* menu);


/* 主程序调用 */

void MenuInit(void);

void MenuRun(void);


#endif


