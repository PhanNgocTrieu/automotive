/**
 * main_host.c - Mô phỏng scheduler trên PC: 1 giây "ảo" = 1000 tick.
 * Trên MCU: gọi Sched_Tick() trong SysTick_Handler và Sched_Run() trong for(;;).
 */
#include <stdio.h>
#include "sched.h"

static uint32_t c1, c10, c100;

static void Task_1ms(void)   { c1++; }
static void Task_10ms(void)  { c10++; }
static void Task_100ms(void)
{
    c100++;
    if ((c100 % 5u) == 0u) {
        printf("[t=%4u ms] 1ms:%u  10ms:%u  100ms:%u\n",
               (unsigned)Sched_Now(), (unsigned)c1, (unsigned)c10, (unsigned)c100);
    }
}

static SchedTask_t s_tasks[] = {
    /* fn,         period, offset */
    { Task_1ms,    1u,     0u, 0u, 0u },
    { Task_10ms,   10u,    2u, 0u, 0u },   /* offset để không trùng tick với 100ms */
    { Task_100ms,  100u,   5u, 0u, 0u },
};

int main(void)
{
    uint32_t i;
    Sched_Init(s_tasks, (uint8_t)(sizeof(s_tasks) / sizeof(s_tasks[0])));

    for (i = 0u; i < 1000u; i++) {
        Sched_Run();        /* main loop chạy liên tục, thấy tick hiện tại */
        Sched_Tick();       /* giả lập ngắt timer 1 ms */
    }
    printf("Overruns: %u %u %u\n", (unsigned)s_tasks[0].overruns,
           (unsigned)s_tasks[1].overruns, (unsigned)s_tasks[2].overruns);
    return 0;
}
