#ifndef __OLED_H
#define __OLED_H

#include "main.h"

/* SSD1306 分辨率 */
#define OLED_WIDTH   128
#define OLED_HEIGHT  64

/**
 * @brief  初始化 OLED (SSD1306, I2C)
 */
void OLED_Init(void);

/**
 * @brief  清屏
 */
void OLED_Clear(void);

/**
 * @brief  在指定位置显示字符串 (8x16 字体, 每行最多16字符)
 * @param  x: 列 (0~127)
 * @param  y: 行 (0~7, 按8像素行)
 * @param  str: 字符串
 */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);

/**
 * @brief  在指定页显示（0~3页, 对应OLED四行大字模式）
 * @param  line: 行号 0~3
 * @param  str: 字符串 (最多16字符)
 */
void OLED_ShowLine(uint8_t line, const char *str);

/**
 * @brief  显示带小数的一行文本
 * @param  line: 行号 0~3
 * @param  prefix: 前缀文本如 "Temp:"
 * @param  value: 整数值
 * @param  suffix: 后缀如 "C"
 */
void OLED_ShowValue(uint8_t line, const char *prefix, int8_t value, const char *suffix);

/**
  * @brief  将缓冲区内容刷新到 OLED (全屏)
  */
void OLED_Refresh(void);

#endif
