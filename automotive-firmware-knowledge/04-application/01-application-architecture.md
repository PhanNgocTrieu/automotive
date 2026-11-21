# 01 – Kiến trúc tầng Application

## 1. Application layer làm gì?
Thực hiện **chức năng của xe** (feature): điều khiển đèn, gạt mưa, cửa sổ, quạt làm mát, sạc pin… dựa trên input (sensor, CAN signal) và tạo output (actuator, CAN signal). **Không truy cập phần cứng trực tiếp.**

## 2. Nguyên tắc thiết kế
1. **Tách biệt phần cứng:** chỉ dùng interface (`Rte_Read/Write`, hoặc API IoHwAb).
2. **Mỗi SWC một trách nhiệm** (Single Responsibility).
3. **Input → Processing → Output** rõ ràng, chạy theo chu kỳ cố định.
4. **Đơn vị vật lý thống nhất** (ví dụ 0.1°C, mV, rpm) và ghi rõ trong tên/tài liệu: `coolantTemp_dC`, `vbat_mV`.
5. **Xử lý trạng thái lỗi/không hợp lệ** của mọi input (signal invalid, timeout, sensor fault) → giá trị thay thế (substitute value) / degraded mode.
6. **Testable:** logic thuần, có thể chạy trên PC (unit test, SIL).

## 3. Cấu trúc điển hình của một feature

```
              ┌──────────────── Feature: Exterior Light ────────────────┐
 Switch ─►   │  Input SWC          Logic SWC              Output SWC     │ ─► High-side switch
 (DIO)       │  - Debounce          - State machine        - Command     │
 Light sensor│  - Plausibility      - Auto light (hyster.) - Diag output │
 (CAN)  ─►   │  - Timeout handling  - Coming home timer    - Fault → DTC │ ─► CAN status
              └────────────────────────────────────────────────────────┘
```

## 4. Mẫu code (không AUTOSAR, có abstraction)
```c
/* app_light.h */
typedef enum { SW_OFF = 0, SW_AUTO, SW_ON } LightSwitch_t;
void AppLight_Init(void);
void AppLight_Run10ms(void);

/* app_light.c */
#include "app_light.h"
#include "rte.h"            /* Rte_Read_..., Rte_Write_... (tự viết hoặc generated) */

#define LUX_ON_THRESHOLD   (150u)
#define LUX_OFF_THRESHOLD  (300u)    /* hysteresis */

static bool s_autoOn = false;

void AppLight_Init(void) { s_autoOn = false; }

void AppLight_Run10ms(void)
{
    LightSwitch_t sw  = Rte_Read_LightSwitch();
    uint16_t lux      = 0u;
    bool luxValid     = Rte_Read_AmbientLux(&lux);    /* false nếu timeout/invalid */
    bool cmd;

    if (!luxValid) {
        s_autoOn = true;             /* fail-safe: không biết sáng/tối → bật đèn */
    } else if (lux < LUX_ON_THRESHOLD) {
        s_autoOn = true;
    } else if (lux > LUX_OFF_THRESHOLD) {
        s_autoOn = false;
    } else {
        /* giữ nguyên trong vùng hysteresis */
    }

    switch (sw) {
        case SW_ON:   cmd = true;      break;
        case SW_AUTO: cmd = s_autoOn;  break;
        case SW_OFF:
        default:      cmd = false;     break;
    }
    Rte_Write_HeadlightCmd(cmd);
}
```

## 5. Data dictionary
Mỗi signal nội bộ nên có: tên, kiểu, đơn vị, resolution, dải, giá trị mặc định, giá trị invalid, nguồn (producer), consumer. Đây là nền cho ARXML/DBC và cho review.

| Signal | Type | Unit | Range | Default | Invalid | Producer |
|---|---|---|---|---|---|---|
| AmbientLux | uint16 | lux | 0–65000 | 0 | 0xFFFF | RainLightSensor (CAN 0x3A0) |
| HeadlightCmd | bool | – | 0/1 | 0 | – | AppLight |

## 6. Variant & Calibration
- **Variant coding:** cùng phần mềm cho nhiều cấu hình xe (có/không đèn LED, tay lái trái/phải) → đọc từ NvM (ghi bằng UDS 0x2E tại nhà máy).
- **Calibration parameters:** ngưỡng, thời gian, bảng tra → đặt ở vùng calibration, chỉnh bằng XCP (INCA/CANape) mà không build lại.
```c
const volatile uint16_t CAL_LuxOnThreshold __attribute__((section(".calib"))) = 150u;
```

---

## Câu hỏi tự kiểm tra
1. Vì sao application không gọi trực tiếp `Dio_WriteChannel`?
2. Hysteresis trong auto-light để làm gì?
3. Nêu chiến lược fail-safe khi mất tín hiệu cảm biến ánh sáng.
4. Variant coding khác calibration như thế nào?
