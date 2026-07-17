/**
 * 温控模块 (回差控制 + PWM 输出)
 * TIM3_CH1 (PA6) 驱动 IRF520 MOS管
 */

#include "control.h"
#include "tim.h"

/* PWM 占空比: 0=停止, 999=全速 */
#define PWM_MAX     999
#define PWM_OFF     0

static int8_t  target_temp = TEMP_DEFAULT;  /* 目标温度 */
static uint8_t is_cooling  = 0;             /* 制冷状态标志 */

/* 回差区间 */
#define HYSTERESIS  1   /* 回差 1℃: >target+1 开, <=target 关 */

void Control_Init(void)
{
    /* 启动 PWM 通道 */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    /* 初始关闭制冷 */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM_OFF);
    is_cooling = 0;
    target_temp = TEMP_DEFAULT;
}

void Control_SetTarget(int8_t temp)
{
    if (temp < TEMP_MIN) temp = TEMP_MIN;
    if (temp > TEMP_MAX) temp = TEMP_MAX;
    target_temp = temp;

    /* 立即根据新目标更新制冷状态 */
    /* (下次 Control_Update 会重新判断) */
}

int8_t Control_GetTarget(void)
{
    return target_temp;
}

void Control_Update(int8_t current_temp)
{
    if (current_temp > target_temp + HYSTERESIS) {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM_MAX);
        is_cooling = 1;
    } else if (current_temp <= target_temp) {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM_OFF);
        is_cooling = 0;
    }
}

uint8_t Control_IsCooling(void)
{
    return is_cooling;
}
