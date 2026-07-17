/**
 * DHT11 驱动 (STM32F103 HAL库)
 * 引脚: PA0, 输入上拉
 * 协议: 单总线 (One-Wire)
 */

#include "dht11.h"

static void DHT11_DelayUs(uint16_t us)
{
    uint32_t count = us * 8;
    while (count--) {
        __NOP();
    }
}

#define DHT11_DQ_H()    (GPIOA->BSRR = 0x0001)
#define DHT11_DQ_L()    (GPIOA->BRR = 0x0001)
#define DHT11_DQ_READ() ((GPIOA->IDR & 0x0001) != 0)

static void DHT11_SetOutput(void)
{
    uint32_t crl = GPIOA->CRL;
    crl &= ~(0xFUL << 0);
    crl |=  (0x5UL << 0);
    GPIOA->CRL = crl;
}

static void DHT11_SetInput(void)
{
    uint32_t crl = GPIOA->CRL;
    crl &= ~(0xFUL << 0);
    crl |=  (0x8UL << 0);
    GPIOA->CRL = crl;
}

static uint8_t DHT11_ReadByte(void)
{
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        byte <<= 1;
        uint16_t timeout = 0;
        while (!DHT11_DQ_READ()) {
            if (++timeout > 500) return 0;
        }
        DHT11_DelayUs(40);
        if (DHT11_DQ_READ()) {
            byte |= 1;
            timeout = 0;
            while (DHT11_DQ_READ()) {
                if (++timeout > 500) break;
            }
        }
    }
    return byte;
}

uint8_t DHT11_Read(DHT11_Data *data)
{
    uint8_t buf[5] = {0};
    uint16_t timeout = 0;

    if (data == NULL) return 1;

    DHT11_SetOutput();
    DHT11_DQ_L();
    HAL_Delay(20);

    DHT11_DQ_H();
    DHT11_DelayUs(30);

    DHT11_SetInput();

    timeout = 0;
    while (DHT11_DQ_READ()) {
        if (++timeout > 500) return 1;
    }
    timeout = 0;
    while (!DHT11_DQ_READ()) {
        if (++timeout > 500) return 1;
    }
    timeout = 0;
    while (DHT11_DQ_READ()) {
        if (++timeout > 500) return 1;
    }

    for (uint8_t i = 0; i < 5; i++) {
        buf[i] = DHT11_ReadByte();
    }

    uint8_t checksum = buf[0] + buf[1] + buf[2] + buf[3];
    if (checksum != buf[4]) {
        return 1;
    }

    data->humidity    = buf[0];
    data->temperature = buf[2];

    return 0;
}