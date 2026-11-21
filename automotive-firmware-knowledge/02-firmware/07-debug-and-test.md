# 07 – Debug & Test Firmware

## 1. Công cụ debug
| Tool | Ghi chú |
|---|---|
| ST-Link, J-Link, CMSIS-DAP | Cortex-M, rẻ |
| **Lauterbach TRACE32** | Chuẩn công nghiệp automotive (TriCore, RH850, multi-core, trace) |
| PLS UDE, iSYSTEM winIDEA | TriCore, automotive |
| OpenOCD + GDB | Miễn phí |
| VS Code + Cortex-Debug | Giao diện tiện |

Giao diện: **JTAG**, **SWD** (2 dây), **DAP** (Infineon), **LPD** (Renesas).

### Kỹ thuật debug cơ bản
- Breakpoint (hardware có hạn: 4–8 cái), watchpoint (dừng khi biến bị ghi).
- Xem register ngoại vi qua **SVD file**.
- Live watch (đọc RAM khi đang chạy).
- **Semihosting / SWO / RTT (Segger)** để log mà không cần UART.
- Trace (ETM/ITM, MCDS trên AURIX): xem lịch sử lệnh, đo timing không xâm lấn.

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg &
arm-none-eabi-gdb app.elf
(gdb) target extended-remote :3333
(gdb) monitor reset halt
(gdb) load
(gdb) break main
(gdb) continue
(gdb) info registers
(gdb) x/16xw 0x20000000
```

## 2. Debug HardFault (Cortex-M)
Khi HardFault, đọc các thanh ghi:
- **CFSR** (Configurable Fault Status Register): UFSR / BFSR / MMFSR – loại lỗi.
- **HFSR**, **MMFAR**, **BFAR** (địa chỉ gây lỗi).
- Stack frame: lấy **PC** bị lỗi từ stack (offset 24 byte), tra `.lst`/`addr2line`.

```c
void HardFault_Handler_C(uint32_t *frame)
{
    volatile uint32_t pc  = frame[6];
    volatile uint32_t lr  = frame[5];
    volatile uint32_t cfsr = SCB->CFSR;
    volatile uint32_t bfar = SCB->BFAR;
    (void)pc; (void)lr; (void)cfsr; (void)bfar;
    __BKPT(0);                      /* dừng ở đây khi có debugger */
    for (;;) { }
}
/* Assembly wrapper chọn MSP/PSP:
   tst lr, #4 ; ite eq ; mrseq r0, msp ; mrsne r0, psp ; b HardFault_Handler_C */
```
```bash
arm-none-eabi-addr2line -e app.elf 0x08001234
```
Nguyên nhân phổ biến: NULL pointer, stack overflow, truy cập unaligned (M0), gọi hàm qua con trỏ rác, chia cho 0 (nếu bật trap), truy cập peripheral chưa bật clock.

Production: lưu thông tin fault vào `.noinit` RAM / NvM, reset an toàn, ghi DTC.

## 3. Các mức kiểm thử

| Mức | Môi trường | Mục đích |
|---|---|---|
| **Unit test** | PC (host) hoặc target | Kiểm tra từng hàm/module, mock phần cứng |
| **Integration test** | Target / SIL | Module làm việc cùng nhau |
| **MIL / SIL / PIL** | Simulink, PC, processor | Model / code / code trên chip |
| **HIL** | ECU thật + mô phỏng plant real-time | Kiểm thử hệ thống, fault injection (đứt dây, chập) |
| **Vehicle test** | Xe thật | Nghiệm thu |

## 4. Unit test với Unity + CMock (Ceedling)

Tách phần cứng sau lớp abstraction để test được trên PC:
```c
/* led.c */
#include "dio.h"
void Led_Set(bool on) { Dio_Write(LED_CH, on ? 1u : 0u); }

/* test_led.c */
#include "unity.h"
#include "mock_dio.h"     /* CMock tự generate từ dio.h */
#include "led.h"

void test_Led_On_writes_high(void)
{
    Dio_Write_Expect(LED_CH, 1u);
    Led_Set(true);
}
```
```bash
gem install ceedling
ceedling new mytest && cd mytest
ceedling test:all
ceedling gcov:all     # coverage
```
Kỹ thuật: **boundary value**, **equivalence class**, **fault injection**, test theo requirement.

## 5. Coverage
- **Statement**, **Branch/Decision**, **MC/DC** (mỗi điều kiện con độc lập ảnh hưởng kết quả).
```c
if (a && b)  /* MC/DC cần: (1,1)→T, (0,1)→F, (1,0)→F */
```
- gcov/lcov (miễn phí), VectorCAST, Tessy, Testwell CTC++ (thương mại).

## 6. Test tự động với CAN
- Python `python-can` + `udsoncan` + `pytest` → gửi request, kiểm tra response.
- CANoe + CAPL scripts / vTESTstudio.

## 7. Kỹ thuật debug khi không có debugger
- GPIO toggle + scope/logic analyzer.
- UART log có timestamp (ring buffer, gửi ở background).
- Gửi biến debug qua CAN message riêng.
- XCP để đọc biến RAM thời gian thực.
- Lưu "breadcrumb" (mã checkpoint) vào noinit RAM để biết code dừng ở đâu trước khi reset.

---

## Câu hỏi tự kiểm tra
1. Làm sao tìm dòng code gây HardFault?
2. Tại sao cần abstraction layer để unit test trên PC?
3. HIL giúp gì mà unit test không làm được?
4. Viết test case MC/DC cho `if ((a || b) && c)`.

## Thực hành
- Cố tình tạo HardFault (dereference 0xFFFFFFFF), viết handler in ra PC/CFSR qua UART, dùng addr2line tìm dòng lỗi.
- Thiết lập Ceedling cho module ring buffer và debounce, đạt 100% branch coverage.
