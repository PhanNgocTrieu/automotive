# 06 – Clock, Power mode, Watchdog

## 1. Clock tree
```
IRC (RC nội, ±1–3%)  ─┐
XOSC (thạch anh 8–40 MHz, ±50ppm) ─┼─► PLL (×N /M) ─► SYSCLK ─► AHB ─► APB1/APB2 ─► Peripherals
LPO (32–128 kHz)     ─┘                                    (prescaler)
```
Cấu hình (AUTOSAR **Mcu**: `Mcu_Init`, `Mcu_InitClock`, `Mcu_GetPllStatus`, `Mcu_DistributePllClock`):
1. Bật XOSC, chờ ổn định (có timeout!).
2. Cấu hình PLL, chờ lock.
3. Tăng **flash wait states** *trước* khi tăng tần số.
4. Chọn PLL làm system clock.
5. Bật **clock monitor (CMU)** – phát hiện mất/sai clock → reset hoặc chuyển safe clock (safety mechanism).

Peripheral có clock gating: phải bật clock mới truy cập được register.

## 2. Power modes
| Mode | CPU | Peripheral | RAM | Wake-up | Dòng |
|---|---|---|---|---|---|
| Run | Chạy | Theo cấu hình | Giữ | – | mA |
| Sleep / Wait | Dừng (WFI) | Chạy | Giữ | Bất kỳ IRQ | Giảm |
| Stop / Deep sleep | Dừng | Phần lớn dừng | Giữ | EXTI, RTC, CAN wake | µA |
| Standby / VLPS | Tắt | Tắt | Mất (trừ backup) | Pin wake, RTC | rất thấp |

Thực tế ECU: khi xe tắt, thường **SBC ngắt nguồn MCU hoàn toàn** và chỉ transceiver/SBC thức để chờ wake-up (CAN, KL15, LIN) → dòng tĩnh µA.

Trình tự shutdown (**EcuM / BswM**):
```
KL15 OFF → app báo không cần chạy → ComM yêu cầu No Com → CanNm ready to sleep
→ NvM_WriteAll → (đợi xong) → transceiver sleep → SBC sleep/ ngắt nguồn MCU
```

## 3. Watchdog

### Loại
| Loại | Mô tả |
|---|---|
| Timeout watchdog | Phải refresh trước khi hết thời gian |
| **Window watchdog** | Refresh **quá sớm** hoặc **quá muộn** đều reset → phát hiện cả chạy quá nhanh (vòng lặp bị bỏ qua) |
| Internal WDG | Trong MCU, có clock riêng (LPO) |
| **External WDG** (trong SBC) | Độc lập hoàn toàn với MCU – yêu cầu safety; có thể là **Q&A watchdog** (SBC gửi câu hỏi, MCU tính câu trả lời đúng) |

### Nguyên tắc refresh đúng
❌ Refresh trong ISR timer → CPU vẫn ngắt được dù main loop treo.
❌ Refresh rải rác khắp nơi.
✅ Chỉ refresh khi **tất cả các task quan trọng đã báo "alive"** trong chu kỳ:

```c
#define ALIVE_TASK_1MS   (1u << 0)
#define ALIVE_TASK_10MS  (1u << 1)
#define ALIVE_TASK_100MS (1u << 2)
#define ALIVE_ALL        (ALIVE_TASK_1MS | ALIVE_TASK_10MS | ALIVE_TASK_100MS)

static volatile uint32_t s_alive;

void WdgM_Checkpoint(uint32_t id) { /* gọi từ mỗi task */
    uint32_t pm = __get_PRIMASK(); __disable_irq();
    s_alive |= id;
    __set_PRIMASK(pm);
}

void WdgM_MainFunction(void)       /* gọi mỗi 100 ms, trong cửa sổ hợp lệ */
{
    if (s_alive == ALIVE_ALL) {
        Wdg_Refresh();
        s_alive = 0u;
    }
    /* else: không refresh → reset; có thể ghi lý do vào noinit RAM trước */
}
```
AUTOSAR **WdgM** giám sát: **alive supervision** (tần suất), **deadline supervision** (thời gian giữa 2 checkpoint), **logical supervision** (thứ tự checkpoint).

### Sau reset do watchdog
- Đọc **reset cause** → tăng counter, ghi DTC, có thể vào **limp-home** nếu reset liên tục.

## 4. Reset
Nguồn reset: POR, external pin, WDG, software (`NVIC_SystemReset()`), lockup, clock loss, LVD. Lưu và báo cáo reset cause là yêu cầu phổ biến.

---

## Câu hỏi tự kiểm tra
1. Vì sao phải tăng flash wait state trước khi tăng clock?
2. Window watchdog phát hiện được lỗi gì mà timeout watchdog không?
3. Tại sao không refresh watchdog trong ISR timer?
4. Mô tả trình tự shutdown của ECU.

## Thực hành
- Cấu hình clock từ XOSC + PLL lên tần số tối đa bằng register (không dùng CubeMX). Xuất MCO ra chân và đo bằng scope.
- Bật IWDG/WWDG, cố tình treo một task → quan sát reset, đọc reset flag.
- Đo dòng tiêu thụ ở Run / Sleep / Stop.
