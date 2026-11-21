# Automotive Firmware Knowledge Base

Tài liệu tự học và thực hành dành cho kỹ sư muốn làm **Embedded / Automotive Firmware** (ECU, MCU, AUTOSAR, CAN, Functional Safety...).

Tài liệu đi từ nền tảng điện tử → kiến thức hệ thống ô tô → Firmware → Driver → Application, và kết thúc bằng các bài lab/project thực hành.

---

## Cấu trúc thư mục

```
automotive-firmware-knowledge/
├── README.md                          ← Bạn đang ở đây (tổng quan)
│
├── 00-electronics-fundamentals/       ← Nền tảng điện tử
│   ├── 01-circuit-basics.md           Ohm, Kirchhoff, R/L/C, RC filter
│   ├── 02-semiconductors.md           Diode, BJT, MOSFET, Op-amp
│   ├── 03-digital-logic.md            Logic level, số học nhị phân, flip-flop
│   ├── 04-power-electronics.md        LDO, Buck/Boost, nguồn trên xe 12V/48V
│   ├── 05-signals-and-measurement.md  ADC/DAC, PWM, đo đạc với scope/LA
│   ├── 06-schematic-and-pcb.md        Đọc schematic, datasheet, PCB, EMC
│   └── labs.md                        Bài tập điện tử cơ bản
│
├── 01-automotive-fundamentals/        ← Kiến thức hệ thống ô tô
│   ├── 01-vehicle-ee-architecture.md  ECU, Domain/Zonal architecture
│   ├── 02-in-vehicle-networks.md      CAN, CAN FD, LIN, FlexRay, Ethernet
│   ├── 03-diagnostics-uds-obd.md      UDS (ISO 14229), OBD-II, DTC
│   ├── 04-autosar.md                  Classic / Adaptive AUTOSAR
│   ├── 05-functional-safety.md        ISO 26262, ASIL, safety mechanism
│   ├── 06-cybersecurity.md            ISO/SAE 21434, SecOC, Secure Boot
│   ├── 07-development-process.md      V-Model, ASPICE, requirement, traceability
│   └── 08-coding-standards.md         MISRA C, CERT C, static analysis
│
├── 02-firmware/                       ← Firmware (tầng hệ thống của MCU)
│   ├── 01-mcu-architecture.md         Cortex-M/R, TriCore, RH850, memory map
│   ├── 02-startup-and-linker.md       Reset → main(), linker script, sections
│   ├── 03-embedded-c.md               volatile, bit manipulation, pitfalls
│   ├── 04-rtos-and-osek.md            Scheduler, task, ISR, OSEK/AUTOSAR OS
│   ├── 05-bootloader-and-flashing.md  Bootloader, UDS flashing, A/B, OTA
│   ├── 06-memory-and-nvm.md           Flash, EEPROM emulation, NvM, ECC
│   └── 07-debug-and-test.md           JTAG/SWD, trace, unit test, HIL
│
├── 03-drivers/                        ← Device Driver / MCAL
│   ├── 01-driver-design-principles.md Kiến trúc driver, MCAL, HAL, API
│   ├── 02-gpio-and-interrupts.md      GPIO, NVIC, ISR design
│   ├── 03-timer-pwm-adc.md            Timer, PWM, Input Capture, ADC, DMA
│   ├── 04-uart-spi-i2c.md             Serial protocols & driver
│   ├── 05-can-driver.md               CAN controller, mailbox, filter, bus-off
│   └── 06-clock-power-watchdog.md     Clock tree, low power, WDG
│
├── 04-application/                    ← Application layer
│   ├── 01-application-architecture.md SWC, layered design, interfaces
│   ├── 02-state-machines.md           FSM, mode management
│   ├── 03-signal-processing-control.md Debounce, filter, PID, scaling
│   ├── 04-diagnostic-application.md   DTC, debounce lỗi, freeze frame
│   └── 05-model-based-design.md       Simulink, code generation, MIL/SIL/HIL
│
├── 05-labs-projects/                  ← Thực hành theo cấp độ
│   ├── README.md                      Danh sách lab + project
│   └── code/                          Code mẫu (C)
│
└── 06-references/
    ├── glossary.md                    Thuật ngữ
    └── resources.md                   Sách, chuẩn, khoá học, tool
```

---

## Lộ trình học đề xuất (~6 tháng, 10–15h/tuần)

| Giai đoạn | Thời gian | Nội dung | Kết quả mong đợi |
|---|---|---|---|
| **1. Nền tảng điện tử** | Tuần 1–4 | `00-electronics-fundamentals` | Đọc được schematic, hiểu pull-up, MOSFET switch, RC filter, ADC |
| **2. Embedded C & MCU** | Tuần 5–8 | `02-firmware/01–03` | Viết được bare-metal blink, hiểu startup code, linker |
| **3. Driver** | Tuần 9–13 | `03-drivers` | Tự viết driver GPIO/UART/ADC/Timer/CAN theo register |
| **4. Automotive** | Tuần 14–18 | `01-automotive-fundamentals` | Hiểu CAN/UDS/AUTOSAR/ISO 26262, đọc được DBC |
| **5. RTOS, Bootloader, NvM** | Tuần 19–22 | `02-firmware/04–07` | Ứng dụng nhiều task, bootloader đơn giản |
| **6. Application** | Tuần 23–26 | `04-application` + Capstone | Một "mini-ECU" hoàn chỉnh có CAN + UDS + DTC |

> 👉 Lộ trình chi tiết theo từng tuần, mốc hoàn thành và hướng chuyên sâu: xem **[ROADMAP.md](../ROADMAP.md)** ở thư mục gốc của repo.
>
> Gợi ý: Học song song lý thuyết và lab. Mỗi file đều có phần **"Câu hỏi tự kiểm tra"** và/hoặc **"Thực hành"** ở cuối.

---

## Phần cứng gợi ý để thực hành

| Thiết bị | Mục đích | Ghi chú |
|---|---|---|
| STM32 Nucleo-F446RE / G474RE | MCU Cortex-M4, có bxCAN / FDCAN | Rẻ, tài liệu nhiều |
| NXP S32K144EVB | MCU automotive-grade, có S32 Design Studio + AUTOSAR MCAL | Gần môi trường thực tế |
| Module CAN transceiver (TJA1050 / SN65HVD230 / TJA1051) | Kết nối bus CAN | 2 module để tạo mạng |
| USB-CAN (PCAN-USB, CANable, Kvaser) | Sniff / gửi CAN từ PC | CANable + `can-utils` trên Linux rất rẻ |
| Logic analyzer 8CH (Saleae clone) + PulseView | Debug UART/SPI/I2C/CAN | |
| Oscilloscope (≥ 50 MHz) | Đo tín hiệu analog, PWM | |
| Multimeter, breadboard, linh kiện R/C/LED/MOSFET | Lab điện tử | |
| Nguồn DC điều chỉnh 0–30V | Mô phỏng nguồn 12V xe | |

Không có phần cứng? Có thể bắt đầu với **QEMU** (Cortex-M), **Renode**, và **vcan** trên Linux (`sudo modprobe vcan`).

---

## Cách sử dụng

1. Đọc theo thứ tự số trong mỗi thư mục.
2. Làm phần thực hành cuối mỗi bài, ghi chép lại vào thư mục `notes/` của riêng bạn.
3. Code mẫu nằm trong `05-labs-projects/code/` — đọc, build, sửa và mở rộng.
4. Tra thuật ngữ trong `06-references/glossary.md`.
