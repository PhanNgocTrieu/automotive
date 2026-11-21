# 01 – Kiến trúc MCU

## 1. MCU automotive phổ biến

| Hãng | Dòng | Core | Ghi chú |
|---|---|---|---|
| Infineon | **AURIX TC2xx/TC3xx/TC4xx** | TriCore (lockstep) | Powertrain, chassis, ASIL D, rất phổ biến |
| Infineon | Traveo T2G | Cortex-M4/M7 | Body, cluster |
| NXP | **S32K1/S32K3** | Cortex-M0+/M4F/M7 (lockstep) | Body, general purpose, dễ học |
| NXP | S32G, S32Z/E | Cortex-A53 + M7 / R52 | Gateway, zonal |
| NXP | MPC57xx | Power Architecture e200 | Cũ hơn, vẫn nhiều |
| Renesas | **RH850** (F1x, P1x, U2x) | G3/G4 core | Rất phổ biến ở OEM Nhật |
| ST | **SPC5 / Stellar** | Power Arch / Cortex-R52 | |
| TI | TMS570 / Hercules | Cortex-R4F/R5F lockstep | Safety |
| Microchip | dsPIC, SAM | | |

Khi học: **STM32 (Cortex-M)** rẻ & nhiều tài liệu → sau đó chuyển sang **S32K144** để quen môi trường automotive.

## 2. Thành phần bên trong MCU

```
┌─────────────────────────── MCU ──────────────────────────────┐
│  CPU Core(s) ── NVIC/Interrupt ctrl ── MPU ── Debug (SWD/JTAG)│
│      │                                                        │
│  ═══ Bus matrix (AHB/AXI, crossbar) ════════════════════════  │
│      │           │          │          │         │            │
│   Flash (code) SRAM      DMA      Peripheral bridge (APB)     │
│   + ECC        + ECC              │   │   │   │   │   │       │
│                                 GPIO ADC TIM UART SPI CAN ... │
│  Clock: IRC, XOSC, PLL ── Clock monitor                       │
│  Power: regulator, LVD, low-power modes   Watchdog   HSM      │
└───────────────────────────────────────────────────────────────┘
```

## 3. ARM Cortex-M cơ bản

| Core | Đặc điểm |
|---|---|
| M0/M0+ | Đơn giản, ARMv6-M, rẻ |
| M3 | ARMv7-M |
| M4(F) | + DSP, FPU single precision |
| M7 | Cache, FPU double, hiệu năng cao |
| M33 | ARMv8-M, TrustZone |
| R5/R52 | Real-time, lockstep, safety |

### Thanh ghi core
- R0–R12 general purpose; **R13 = SP** (MSP/PSP), **R14 = LR**, **R15 = PC**, **xPSR**.
- **AAPCS (calling convention):** R0–R3 truyền tham số, R0 trả về; R4–R11 callee-saved.

### Exception & Interrupt
- **Vector table** ở đầu flash (hoặc relocate qua **VTOR**): entry 0 = giá trị SP ban đầu, entry 1 = Reset_Handler, sau đó NMI, HardFault, MemManage, BusFault, UsageFault, SVC, PendSV, SysTick, rồi IRQ0…N.
- **NVIC:** enable/disable, priority (số nhỏ = ưu tiên cao), preemption & sub-priority, tail-chaining, late-arrival.
- Khi vào exception, phần cứng tự **stack** R0–R3, R12, LR, PC, xPSR → ISR có thể viết bằng C thường.
- **PendSV + SysTick:** nền tảng context switch của RTOS.

### Chế độ
- Thread mode / Handler mode; Privileged / Unprivileged.
- MSP (dùng cho handler, OS) / PSP (dùng cho task).

## 4. Memory map

Cortex-M (ví dụ STM32F4):
| Địa chỉ | Vùng |
|---|---|
| 0x0000_0000 | Alias boot (flash/system memory/SRAM) |
| 0x0800_0000 | Flash |
| 0x2000_0000 | SRAM |
| 0x4000_0000 | Peripherals |
| 0xE000_0000 | Private Peripheral Bus: NVIC, SysTick, SCB, debug |

→ **Memory-mapped I/O:** register ngoại vi chỉ là địa chỉ bộ nhớ:
```c
#define GPIOA_BASE   (0x40020000UL)
#define GPIOA_ODR    (*(volatile uint32_t *)(GPIOA_BASE + 0x14UL))
GPIOA_ODR |= (1UL << 5);   /* PA5 = 1 */
```
Thực tế dùng header **CMSIS** do hãng cung cấp (`GPIOA->ODR`).

## 5. Bit-banding / atomic set-clear
Read-modify-write (`REG |= x`) không nguyên tử → có thể bị ISR chen ngang. Dùng register set/clear chuyên dụng: `GPIOx->BSRR` (STM32), `PTx->PSOR/PCOR` (S32K), hoặc bit-band (M3/M4).

## 6. Cache & Memory barrier (Cortex-M7, Cortex-A)
- D-Cache có thể làm DMA thấy dữ liệu cũ → clean/invalidate cache, hoặc đặt buffer DMA ở vùng non-cacheable (MPU).
- `__DSB()`, `__ISB()`, `__DMB()` – đảm bảo thứ tự truy cập bộ nhớ (ví dụ sau khi ghi VTOR, sau khi clear cờ ngắt trước khi return).

## 7. Multi-core (AURIX, S32K3, S32G)
- Mỗi core có local RAM (DSPR/PSPR trên TriCore), shared RAM (LMU).
- Đồng bộ: spinlock, hardware semaphore, inter-core interrupt.
- Phân chia: core 0 chạy BSW/Com, core 1 chạy safety app, core 2 chạy QM...

---

## Câu hỏi tự kiểm tra
1. Entry đầu tiên của vector table Cortex-M chứa gì?
2. Phần cứng tự lưu những thanh ghi nào khi vào ISR?
3. Tại sao `GPIOA->ODR |= x` có thể gây race condition? Giải pháp?
4. Khi dùng DMA trên Cortex-M7 có D-cache cần chú ý gì?

## Thực hành
- Đọc **"The Definitive Guide to ARM Cortex-M3/M4"** (Joseph Yiu) chương 4, 7, 8.
- Trên Nucleo: in ra giá trị `SCB->CPUID`, đọc vector table bằng debugger.
