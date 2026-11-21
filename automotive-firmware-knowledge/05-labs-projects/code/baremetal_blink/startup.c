/**
 * startup.c - Startup code tối giản cho STM32F446 (Cortex-M4), không dùng CMSIS/HAL.
 * Lab M1 – xem 02-firmware/02-startup-and-linker.md
 */
#include <stdint.h>

extern uint32_t _sidata;   /* địa chỉ LMA của .data trong Flash */
extern uint32_t _sdata, _edata;
extern uint32_t _sbss, _ebss;
extern uint32_t _estack;

extern int main(void);
void SysTick_Handler(void);

void Reset_Handler(void);
void Default_Handler(void);

void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));

/* Vector table: 16 exception của core (chưa liệt kê IRQ ngoại vi cho gọn) */
__attribute__((section(".isr_vector"), used))
void (* const g_vectors[16])(void) = {
    (void (*)(void))(&_estack),   /* 0: Initial MSP */
    Reset_Handler,                /* 1: Reset */
    NMI_Handler,                  /* 2 */
    HardFault_Handler,            /* 3 */
    MemManage_Handler,            /* 4 */
    BusFault_Handler,             /* 5 */
    UsageFault_Handler,           /* 6 */
    0, 0, 0, 0,                   /* 7-10: reserved */
    SVC_Handler,                  /* 11 */
    0,                            /* 12: Debug monitor */
    0,                            /* 13: reserved */
    PendSV_Handler,               /* 14 */
    SysTick_Handler,              /* 15 */
};

void Reset_Handler(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst;

    /* 1. Copy .data Flash → RAM */
    for (dst = &_sdata; dst < &_edata; ) {
        *dst++ = *src++;
    }
    /* 2. Xoá .bss */
    for (dst = &_sbss; dst < &_ebss; ) {
        *dst++ = 0u;
    }
    /* 3. (Tuỳ chọn) SystemInit: clock/PLL, FPU. Ở đây chạy HSI 16 MHz mặc định. */

    (void)main();
    for (;;) { }
}

void Default_Handler(void)
{
    for (;;) { }    /* đặt breakpoint ở đây khi debug */
}
