#include "menu.h"

/* 宏定义 */
#define FONT_HEIGHT         24
#define SCREEN_HEIGHT       172
#define VISIBLE_ITEMS       (SCREEN_HEIGHT / FONT_HEIGHT)

/* 声明菜单 */
Menu_t menu;

/* 私有函数声明 */
static void _Menu_LinkItems(MenuItem_t* items, size_t count, MenuItem_t* parent);
static void _Menu_Next(Menu_t* menu);
static void _Menu_Previous(Menu_t* menu);
static void _Menu_Select(Menu_t* menu);
static void _Menu_Back(Menu_t* menu);


/* ------------------------------公共API------------------------------ */

/* 创建菜单 */
void Menu_Create(Menu_t* menu, MenuItem_t* rootItems, size_t rootItemCount) {
    // 链接所有菜单项
    _Menu_LinkItems(rootItems, rootItemCount, NULL);

    // 初始化菜单状态
    menu->rootMenu = rootItems;
    menu->currentMenu = rootItems;
    menu->currentSelection = rootItems;
    menu->topVisibleItem = rootItems;
    menu->needsRedraw = 1; // 初始时需要绘制
}

/* 菜单事件接收 */
void Menu_HandleInput(Menu_t* menu, MenuInput_e input) {
    switch (input) {
        case MENU_INPUT_NEXT:
            _Menu_Next(menu);
            break;
        case MENU_INPUT_PREVIOUS:
            _Menu_Previous(menu);
            break;
        case MENU_INPUT_SELECT:
            _Menu_Select(menu);
            break;
        case MENU_INPUT_BACK:
            _Menu_Back(menu);
            break;
    }
}

/* 绘制菜单 */
void Menu_Draw(Menu_t* menu,lcd* plcd) {
    if (!menu->needsRedraw) {
        return; // 如果不需要重绘，直接返回
    }

    // --- 核心滚动逻辑 ---
    // 计算当前菜单、选中项和屏幕顶项的索引
    int currentIndex = 0;
    int selectedIndex = -1;
    int topIndex = -1;

    MenuItem_t* p = menu->currentMenu;
    while(p) {
        if (p == menu->currentSelection) selectedIndex = currentIndex;
        if (p == menu->topVisibleItem) topIndex = currentIndex;
        p = p->next;
        currentIndex++;
    }

    // 向上滚动
    if (selectedIndex < topIndex) {
        menu->topVisibleItem = menu->currentSelection;
    } 
    // 向下滚动
    else if (selectedIndex >= topIndex + VISIBLE_ITEMS) {
        menu->topVisibleItem = menu->currentSelection;
        // 回溯，让选中项成为可见的最后一项
        for(int i = 0; i < VISIBLE_ITEMS - 1; i++) {
            if(menu->topVisibleItem->prev) {
                menu->topVisibleItem = menu->topVisibleItem->prev;
            } else {
                break;
            }
        }
    }
    
    // --- 绘制 ---
    lcd_clear(plcd, BLACK);
    
    MenuItem_t* itemToDraw = menu->topVisibleItem;
    for (int i = 0; i < VISIBLE_ITEMS; i++) {
        if (itemToDraw) {
            uint8_t y_pos = i * FONT_HEIGHT;
            uint8_t isSelected = (itemToDraw == menu->currentSelection);
            // 假设的OLED绘图函数，注意参数可能需要调整以匹配你的库
			if(isSelected) {
				lcd_set_font(plcd, FONT_2412, LIGHTBLUE, DARKBLUE);
				lcd_print(plcd, 25, y_pos, itemToDraw->text);
				lcd_set_font(plcd, FONT_2412, LIGHTBLUE, BLACK);
			}
			else {
				lcd_print(plcd, 25, y_pos, itemToDraw->text);
			}
//            OLED_PrintString(0, y_pos, itemToDraw->text, &font16x16, isSelected); 
            itemToDraw = itemToDraw->next;
        } else {
            break; // 没有更多项可以绘制
        }
    }
    
    menu->needsRedraw = 0; // 重绘完成，清除标志
}

/* 刷屏请求 */
void Menu_RequestRedraw(Menu_t* menu) {
    menu->needsRedraw = 1;
}


/* ------------------------------私有函数实现------------------------------ */

/**
 * @brief 递归链接菜单项数组，构建双向链表和父子关系
 * @param items 菜单项数组
 * @param count 数组中的项数
 * @param parent 这些项的父菜单项 (顶层菜单为 NULL)
 */
static void _Menu_LinkItems(MenuItem_t* items, size_t count, MenuItem_t* parent) {
    if (!items || count == 0) return;

    for (size_t i = 0; i < count; i++) {
        items[i].parent = parent;
        items[i].prev = (i > 0) ? &items[i - 1] : NULL;
        items[i].next = (i < count - 1) ? &items[i + 1] : NULL;

        // 如果有子菜单，递归链接
        if (items[i].subMenu && items[i].subMenuSize > 0) {
            _Menu_LinkItems(items[i].subMenu, items[i].subMenuSize, &items[i]);
        }
    }
}

static void _Menu_Next(Menu_t* menu) {
    if (menu->currentSelection->next) {
        menu->currentSelection = menu->currentSelection->next;
    } else {
        // 循环到第一个
        menu->currentSelection = menu->currentMenu;
    }
    menu->needsRedraw = 1;
}

static void _Menu_Previous(Menu_t* menu) {
    if (menu->currentSelection->prev) {
        menu->currentSelection = menu->currentSelection->prev;
    } else {
        // 循环到最后一个
        MenuItem_t* last = menu->currentMenu;
        while (last->next) {
            last = last->next;
        }
        menu->currentSelection = last;
    }
    menu->needsRedraw = 1;
}

static void _Menu_Select(Menu_t* menu) {
    MenuItem_t* selection = menu->currentSelection;

    if (selection->actionCallback) {
        selection->actionCallback();
    } else if (selection->subMenu) {
        menu->currentMenu = selection->subMenu;
        menu->currentSelection = selection->subMenu;
        menu->topVisibleItem = selection->subMenu; // 进入子菜单时，滚动条也重置
        menu->needsRedraw = 1;
    }
}

static void _Menu_Back(Menu_t* menu) {
    if (menu->currentMenu->parent) {
        MenuItem_t* parentItem = menu->currentMenu->parent;
        
        // 找到父菜单的头节点
        MenuItem_t* parentMenuHead = parentItem;
        while(parentMenuHead->prev) {
            parentMenuHead = parentMenuHead->prev;
        }

        menu->currentMenu = parentMenuHead;
        menu->currentSelection = parentItem;      // 返回时高亮父项
        menu->topVisibleItem = parentMenuHead;    // 返回父菜单时，滚动条也重置
        menu->needsRedraw = 1;
    }
}

/* ------------------------------菜单内容设定------------------------------ */


/* --------------------回调函数-------------------- */
void action_ShowInfo() { printf("\n>>> ACTION: Showing System Info... <<<\n"); }
void action_SetBrightness() { printf("\n>>> ACTION: Setting Brightness... <<<\n"); }
void action_Reboot() { printf("\n>>> ACTION: Rebooting System... <<<\n"); }
void action_State() { printf("\n>>> ACTION: State System... <<<\n"); }
void action_Func() { printf("\n>>> ACTION: Function System... <<<\n"); }

/* ------------------------------------------------ */


// Settings 子菜单项
MenuItem_t settingsMenuItems[] = {
    { "About",    .subMenu = NULL, .actionCallback = NULL }, // 假设"About"暂时没功能
    { "Display",  .subMenu = NULL, .actionCallback = action_SetBrightness },
    { "Reboot",   .subMenu = NULL, .actionCallback = action_Reboot },
};

// 主菜单项
MenuItem_t mainMenuItems[] = {
    { "System Info",  .subMenu = NULL,               .actionCallback = action_ShowInfo },
    { "Settings",     .subMenu = settingsMenuItems,  .subMenuSize = sizeof(settingsMenuItems)/sizeof(MenuItem_t) },
    { "System State", .subMenu = NULL,               .actionCallback = action_State },
    { "Function",     .subMenu = NULL,               .actionCallback = action_Func },
    { "Another Item", .subMenu = NULL,               .actionCallback = NULL },
    { "More Items",   .subMenu = NULL,               .actionCallback = NULL },
    { "Final Item",   .subMenu = NULL,               .actionCallback = NULL },
};

/* 菜单初始化 */
void MenuInit(void) {
    // 创建菜单并自动完成所有链接
    Menu_Create(&menu, mainMenuItems, sizeof(mainMenuItems)/sizeof(MenuItem_t));
}
/* 运行菜单 */
void MenuRun() {
	Menu_Draw(&menu,&lcd_desc);
}


