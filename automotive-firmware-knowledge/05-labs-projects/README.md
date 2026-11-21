# 05 – Labs & Projects

Các bài thực hành được sắp xếp theo cấp độ. Mỗi lab ghi rõ kiến thức liên quan để quay lại đọc.

## Level 0 – Điện tử (xem `00-electronics-fundamentals/labs.md`)
E1–E10: LED, cầu phân áp, bouncing, RC, MOSFET, PWM, NTC, dòng sleep, logic analyzer, đọc schematic.

## Level 1 – Embedded C trên PC (không cần phần cứng)
| # | Lab | Liên quan | Code |
|---|---|---|---|
| C1 | Ring buffer + unit test | 02-firmware/03 | `code/host_modules/` |
| C2 | Debounce, moving average, IIR | 04-application/03 | `code/host_modules/` |
| C3 | Lookup table nội suy | 04-application/03 | `code/host_modules/` |
| C4 | CRC8 SAE J1850, pack/unpack CAN signal Intel | 02-firmware/06, 03-drivers/05 | `code/host_modules/` |
| C5 | Cooperative scheduler | 02-firmware/04 | `code/scheduler/` |
| C6 | State machine cửa sổ điện + test mọi transition | 04-application/02 | (tự làm) |
| C7 | EEPROM emulation trên flash mô phỏng + test mất điện | 02-firmware/06 | (tự làm) |

```bash
cd code/host_modules && make test
cd code/scheduler && make run
```

## Level 2 – Bare-metal MCU (STM32 Nucleo-F446RE hoặc tương đương)
| # | Lab | Liên quan |
|---|---|---|
| M1 | Bare-metal blink: startup + linker tự viết, Makefile (`code/baremetal_blink/`) | 02-firmware/02 |
| M2 | Driver GPIO register-level + EXTI nút nhấn | 03-drivers/02 |
| M3 | SysTick/Timer 1 ms + scheduler (port C5 lên MCU) | 03-drivers/03, 02-firmware/04 |
| M4 | UART interrupt + ring buffer + CLI | 03-drivers/04 |
| M5 | ADC + DMA, đọc biến trở & NTC, lọc | 03-drivers/03 |
| M6 | PWM dimming LED + input capture | 03-drivers/03 |
| M7 | SPI đọc chip ID (IMU / SPI flash / MCP2515) | 03-drivers/04 |
| M8 | Clock tree từ XOSC + PLL; watchdog; đọc reset reason | 03-drivers/06 |
| M9 | HardFault handler + addr2line | 02-firmware/07 |
| M10 | FreeRTOS: 3 task + queue + mutex, quan sát priority inversion | 02-firmware/04 |

## Level 3 – Automotive
| # | Lab | Liên quan |
|---|---|---|
| A1 | SocketCAN trên Linux: vcan, candump, cansend, cangen | 01-automotive/02 |
| A2 | Viết DBC, encode/decode bằng Python `cantools` | 01-automotive/02 |
| A3 | CAN driver register-level (bxCAN/FDCAN/FlexCAN), giao tiếp với PC qua CANable | 03-drivers/05 |
| A4 | "Com mini": bảng message, gửi chu kỳ, Rx timeout → substitute value | 03-drivers/05 |
| A5 | ISO-TP (Single/First/Consecutive/Flow Control) trên MCU | 01-automotive/02 |
| A6 | UDS server: 0x10, 0x11, 0x22, 0x27 (seed/key), 0x3E, NRC, S3 timeout | 01-automotive/03 |
| A7 | Mini-Dem: event debounce, DTC status byte, 0x19 02 / 0x14 | 04-application/04 |
| A8 | Bootloader qua UART → nâng cấp thành UDS flashing qua CAN (0x34/36/37) | 02-firmware/05 |
| A9 | NXP S32K144 + S32DS + RTD MCAL: Port/Dio/Adc/Can bằng API AUTOSAR | 01-automotive/04 |
| A10 | Safety: RAM test, Flash CRC, program flow monitor, E2E (CRC + counter) | 01-automotive/05 |

Kiểm thử UDS từ PC:
```bash
pip install python-can can-isotp udsoncan cantools
```
```python
import can, isotp, udsoncan
from udsoncan.connections import PythonIsoTpConnection
from udsoncan.client import Client

bus = can.interface.Bus(channel='can0', interface='socketcan')
addr = isotp.Address(isotp.AddressingMode.Normal_11bits, txid=0x7E0, rxid=0x7E8)
stack = isotp.CanStack(bus, address=addr)
with Client(PythonIsoTpConnection(stack), request_timeout=2) as client:
    client.change_session(3)
    resp = client.read_data_by_identifier(0xF190)   # cần cấu hình codec cho DID trong config
    print(resp)
```

## Level 4 – Capstone: "Mini Body Control ECU"

**Mục tiêu:** Một ECU hoàn chỉnh trên STM32/S32K với kiến trúc phân lớp kiểu AUTOSAR.

**Chức năng:**
1. Đọc công tắc đèn (OFF/AUTO/ON) qua GPIO + debounce.
2. Nhận độ sáng môi trường qua CAN (ID 0x3A0, chu kỳ 100 ms), timeout 500 ms → fail-safe bật đèn.
3. Điều khiển đèn pha bằng PWM (soft-start), giám sát dòng qua ADC → phát hiện open load / overcurrent.
4. Gửi trạng thái đèn qua CAN (ID 0x2B0, 100 ms) có **E2E (CRC8 + alive counter)**.
5. Giám sát Vbat → under/over-voltage.
6. UDS: 0x10, 0x11, 0x14, 0x19, 0x22 (DID F190 VIN, F189 SW version, 0x0100 vbat), 0x2E (VIN), 0x27, 0x2F (điều khiển đèn), 0x3E.
7. DTC lưu NvM (EEPROM emulation).
8. Watchdog dựa trên alive checkpoint của các task.
9. Bootloader UDS để cập nhật firmware.

**Kiến trúc thư mục đề xuất:**
```
mini_bcm/
├── app/            light_ctrl.c, vbat_monitor.c, diag_monitors.c
├── rte/            rte.c/.h  (Rte_Read_/Rte_Write_ tự viết)
├── services/       com.c, isotp.c, uds.c (Dcm), dem.c, nvm.c, wdgm.c, ecum.c, sched.c
├── ecual/          iohwab.c, can_if.c
├── mcal/           mcu.c, port.c, dio.c, adc.c, pwm.c, gpt.c, can.c, fls.c, wdg.c
├── config/         *_cfg.h/.c, dbc/mini_bcm.dbc
├── bootloader/
├── test/unit/      Ceedling tests (chạy trên PC)
├── test/integration/ pytest + python-can + udsoncan
├── docs/           requirements.md, architecture.md, pin_mapping.md
└── Makefile / CMakeLists.txt
```

**Tiêu chí hoàn thành:**
- [ ] Requirement có ID, mỗi requirement có ít nhất một test.
- [ ] Unit test cho app/ và services/ đạt ≥ 90% branch coverage.
- [ ] cppcheck (MISRA addon) không còn lỗi nghiêm trọng, deviation có lý do.
- [ ] CPU load đo được < 60%, stack watermark có dư ≥ 30%.
- [ ] Test integration UDS tự động chạy pass.
- [ ] Rút CAN → DTC lost communication + fail-safe; cắm lại → hồi phục.
