#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "main.h"

/* 按键事件 */
typedef enum {
    KEY_NONE = 0,
    KEY_UP_SHORT,     /* 加键短按 */
    KEY_DOWN_SHORT,   /* 减键短按 */
} KeyEvent;

/**
 * @brief  按键扫描 (SysTick 中断中调用, 1ms 一次, 内部 10ms 节拍)
 * @note   在 stm32f1xx_it.c 的 SysTick_Handler 中调用
 */
void Key_Scan_ISR(void);

/**
 * @brief  从事件队列中取出一个按键事件 (主循环中调用)
 * @retval 按键事件 (KEY_NONE 表示队列空)
 */
KeyEvent Key_GetEvent(void);

#endif
