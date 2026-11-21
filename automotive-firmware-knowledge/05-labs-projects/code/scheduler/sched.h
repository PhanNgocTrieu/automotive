/**
 * sched.h - Cooperative time-triggered scheduler (Lab C5 / M3)
 *
 * - Sched_Tick() được gọi từ ISR timer mỗi 1 ms (trên PC: mô phỏng).
 * - Sched_Run() gọi trong vòng lặp chính, chạy task đến hạn (run-to-completion).
 * - Offset giúp dàn trải task để không dồn vào cùng một tick (giảm jitter / peak load).
 */
#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

typedef void (*SchedTaskFn_t)(void);

typedef struct {
    SchedTaskFn_t fn;
    uint32_t period_ms;
    uint32_t offset_ms;
    uint32_t nextRun_ms;   /* nội bộ */
    uint32_t overruns;     /* số lần task bị trễ >= 1 chu kỳ */
} SchedTask_t;

void     Sched_Init(SchedTask_t *tasks, uint8_t count);
void     Sched_Tick(void);              /* gọi từ ISR 1 ms */
void     Sched_Run(void);               /* gọi trong main loop */
uint32_t Sched_Now(void);

#endif /* SCHED_H */
