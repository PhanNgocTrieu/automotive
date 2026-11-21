/**
 * main.c - Blink LED LD2 (PA5) trên Nucleo-F446RE bằng register, SysTick 1 ms.
 * Không dùng HAL/CMSIS: mọi địa chỉ lấy từ Reference Manual RM0390.
 */
#include <stdint.h>

#define REG32(addr)      (*(volatile uint32_t *)(addr))

/* RCC */
#define RCC_BASE         (0x40023800UL)
#define RCC_AHB1ENR      REG32(RCC_BASE + 0x30UL)
#define RCC_AHB1ENR_GPIOAEN (1UL << 0)

/* GPIOA */
#define GPIOA_BASE       (0x40020000UL)
#define GPIOA_MODER      REG32(GPIOA_BASE + 0x00UL)
#define GPIOA_BSRR       REG32(GPIOA_BASE + 0x18UL)
#define GPIOA_ODR        REG32(GPIOA_BASE + 0x14UL)

/* SysTick (Cortex-M core) */
#define SYST_CSR         REG32(0xE000E010UL)
#define SYST_RVR         REG32(0xE000E014UL)
#define SYST_CVR         REG32(0xE000E018UL)

#define CPU_HZ           (16000000UL)     /* HSI mặc định sau reset */
#define LED_PIN          (5u)

static volatile uint32_t s_ms = 0u;

void SysTick_Handler(void)
{
    s_ms++;
}

static void delay_ms(uint32_t ms)
{
    uint32_t start = s_ms;
    while ((s_ms - start) < ms) {
        /* có thể dùng __WFI() để tiết kiệm năng lượng */
    }
}

int main(void)
{
    /* 1. Bật clock GPIOA */
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC_AHB1ENR;                      /* đọc lại: đảm bảo clock đã bật trước khi truy cập */

    /* 2. PA5 = output (MODER5 = 01) */
    GPIOA_MODER = (GPIOA_MODER & ~(3UL << (LED_PIN * 2u))) | (1UL << (LED_PIN * 2u));

    /* 3. SysTick 1 ms, clock = CPU */
    SYST_RVR = (CPU_HZ / 1000UL) - 1UL;
    SYST_CVR = 0u;
    SYST_CSR = (1UL << 2) | (1UL << 1) | (1UL << 0);   /* CLKSOURCE | TICKINT | ENABLE */

    for (;;) {
        if ((GPIOA_ODR & (1UL << LED_PIN)) != 0u) {
            GPIOA_BSRR = (1UL << (LED_PIN + 16u));      /* reset → LED off */
        } else {
            GPIOA_BSRR = (1UL << LED_PIN);              /* set → LED on */
        }
        delay_ms(500u);
    }
}
