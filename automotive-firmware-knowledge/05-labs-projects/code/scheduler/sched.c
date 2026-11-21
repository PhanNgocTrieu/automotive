/**
 * sched.c - Cooperative scheduler
 */
#include "sched.h"

static volatile uint32_t s_tick_ms = 0u;
static SchedTask_t *s_tasks = 0;
static uint8_t s_count = 0u;

void Sched_Init(SchedTask_t *tasks, uint8_t count)
{
    uint8_t i;
    s_tasks = tasks;
    s_count = count;
    for (i = 0u; i < count; i++) {
        s_tasks[i].nextRun_ms = tasks[i].offset_ms;
        s_tasks[i].overruns = 0u;
    }
}

void Sched_Tick(void)
{
    s_tick_ms++;            /* 32-bit, aligned → ghi atomic trên Cortex-M */
}

uint32_t Sched_Now(void)
{
    return s_tick_ms;
}

void Sched_Run(void)
{
    uint8_t i;
    uint32_t now = s_tick_ms;

    for (i = 0u; i < s_count; i++) {
        SchedTask_t *t = &s_tasks[i];
        /* So sánh kiểu "signed difference" → đúng cả khi tick wrap-around sau ~49 ngày */
        if ((int32_t)(now - t->nextRun_ms) >= 0) {
            if ((now - t->nextRun_ms) >= t->period_ms) {
                t->overruns++;                       /* đã lỡ ít nhất một chu kỳ */
                t->nextRun_ms = now;                 /* resync, không chạy bù hàng loạt */
            }
            t->nextRun_ms += t->period_ms;
            t->fn();
        }
    }
}
