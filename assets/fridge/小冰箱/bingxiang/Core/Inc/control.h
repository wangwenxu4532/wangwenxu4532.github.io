#ifndef __CONTROL_H
#define __CONTROL_H

#include "main.h"

/* 温度范围 */
#define TEMP_MIN   0
#define TEMP_MAX   30
#define TEMP_DEFAULT 10

/**
 * @brief  初始化温控模块并启动 PWM
 */
void Control_Init(void);

/**
 * @brief  设置目标温度
 * @param  temp: 目标温度 (5~15℃)
 */
void Control_SetTarget(int8_t temp);

/**
 * @brief  获取当前目标温度
 */
int8_t Control_GetTarget(void);

/**
 * @brief  温控更新 (回差控制 ±0.5℃)
 * @param  current_temp: 当前实际温度
 * @note   每次读到新温度后调用一次
 */
void Control_Update(int8_t current_temp);

/**
 * @brief  查询制冷是否开启
 * @retval 0=关闭, 1=开启
 */
uint8_t Control_IsCooling(void);

#endif
