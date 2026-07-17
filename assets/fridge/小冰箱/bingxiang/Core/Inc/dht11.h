#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"

/* DHT11 数据结构 */
typedef struct {
    uint8_t humidity;    /* 湿度 (0~100%) */
    uint8_t temperature; /* 温度 (0~50℃) */
} DHT11_Data;

/**
 * @brief  读取 DHT11 数据
 * @param  data: 数据指针
 * @retval 0=成功, 1=失败
 * @note   两次调用之间至少间隔 1 秒
 */
uint8_t DHT11_Read(DHT11_Data *data);

#endif
