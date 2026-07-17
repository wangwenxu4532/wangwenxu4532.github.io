#include "bsp_key.h"

#define DEBOUNCE_TICKS    3   /* 连续 3 次扫描 (30ms) 检测到按下才算有效 */
#define KEY_QUEUE_SIZE    8

typedef struct {
    uint8_t count;    /* 连续检测到按下的次数 */
    uint8_t stable;   /* 稳定状态: 1=已触发, 0=空闲 */
} KeyState;

static KeyState key_up   = {0, 0};
static KeyState key_down = {0, 0};

/* 事件队列 (环形缓冲区) */
static KeyEvent  queue[KEY_QUEUE_SIZE];
static volatile uint8_t q_head = 0;
static volatile uint8_t q_tail = 0;

/* 队列压入 (中断安全, 仅写 head) */
static void queue_push(KeyEvent e)
{
    uint8_t next = (q_head + 1) % KEY_QUEUE_SIZE;
    if (next == q_tail) {
        return;  /* 队列满, 丢弃事件 */
    }
    queue[q_head] = e;
    q_head = next;
}

/* 队列取出 (主循环调用, 仅读 tail) */
KeyEvent Key_GetEvent(void)
{
    if (q_head == q_tail) {
        return KEY_NONE;
    }
    KeyEvent e = queue[q_tail];
    q_tail = (q_tail + 1) % KEY_QUEUE_SIZE;
    return e;
}

/* 按键扫描核心逻辑 (中断中调用, 每 10ms 一次) */
static void key_scan_one(uint8_t pressed, KeyState *ks, KeyEvent evt)
{
    if (pressed) {
        if (ks->count < 255) ks->count++;
        if ((ks->count >= DEBOUNCE_TICKS) && (ks->stable == 0)) {
            ks->stable = 1;
            queue_push(evt);
        }
    } else {
        ks->count  = 0;
        ks->stable = 0;
    }
}

/* SysTick 中断中调用 (1ms 一次, 内部 10ms 节拍) */
void Key_Scan_ISR(void)
{
    static uint8_t tick_div = 0;
    if (++tick_div < 10) {
        return;  /* 10ms 节拍 */
    }
    tick_div = 0;
    
    uint8_t up_val   = (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,   KEY_UP_Pin)   == GPIO_PIN_RESET);
    uint8_t down_val = (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET);
    
    key_scan_one(up_val,   &key_up,   KEY_UP_SHORT);
    key_scan_one(down_val, &key_down, KEY_DOWN_SHORT);
}
