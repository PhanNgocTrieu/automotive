# 01 – Kiến trúc E/E (Electrical/Electronic) của xe

## 1. ECU là gì?

**ECU (Electronic Control Unit)**: một hộp điện tử chứa MCU + mạch nguồn + giao tiếp + mạch I/O, điều khiển một chức năng của xe. Xe hiện đại có 50–150 ECU (kiến trúc truyền thống).

Các ECU tiêu biểu:
| ECU | Chức năng |
|---|---|
| ECM / EMS (Engine Control Module) | Phun nhiên liệu, đánh lửa |
| TCU (Transmission Control Unit) | Hộp số |
| BCM (Body Control Module) | Đèn, khoá cửa, gạt mưa, cửa sổ |
| ABS / ESC | Chống bó cứng phanh, cân bằng điện tử |
| EPS | Trợ lực lái điện |
| Airbag (ACU/SRS) | Túi khí – ASIL D |
| IPC / Cluster | Đồng hồ táp-lô |
| IVI / Head unit | Infotainment |
| ADAS (camera, radar) | Hỗ trợ lái |
| BMS | Quản lý pin (EV) |
| MCU/Inverter (EV) | Điều khiển motor kéo |
| OBC / DC-DC | Sạc on-board, chuyển đổi HV → 12V |
| Gateway | Định tuyến giữa các mạng |
| TCU (Telematics) | Kết nối 4G/5G, OTA |

## 2. Cấu trúc bên trong một ECU

```
          ┌──────────────────────────── ECU ─────────────────────────────┐
KL30 ───► │ Protection ─► SBC/Regulator ──► MCU (Core, Flash, RAM)       │
KL15 ───► │                    │ WDG/Reset ─┘    │      │       │        │
          │                    │                 │      │       │        │
CAN  ◄──► │  CAN Transceiver ◄─┴─────────────────┘      │       │        │
LIN  ◄──► │  LIN Transceiver ◄──────────────────────────┘       │        │
Sensors ─►│  Input conditioning (filter, divider) ──► ADC/GPIO   │        │
Loads  ◄──│  Smart high-side switch / H-bridge ◄─────── PWM/GPIO─┘        │
          └───────────────────────────────────────────────────────────────┘
```

## 3. Phân lớp phần mềm trong ECU (tổng quan)

```
┌──────────────────────────────────────┐
│ Application (chức năng: đèn, motor…) │  ← 04-application
├──────────────────────────────────────┤
│ Services: Com, Diag (UDS), NvM, OS,  │  ← 01/02 automotive & firmware
│ Mode management, Bootloader          │
├──────────────────────────────────────┤
│ ECU abstraction / Device drivers     │  ← 03-drivers (SBC, EEPROM ngoài…)
├──────────────────────────────────────┤
│ MCAL (Mcu, Port, Dio, Adc, Pwm, Can…)│  ← 03-drivers
├──────────────────────────────────────┤
│ Hardware (MCU + board)               │  ← 00-electronics
└──────────────────────────────────────┘
```
Đây chính là ý tưởng của **AUTOSAR Classic** (xem `04-autosar.md`).

## 4. Tiến hoá kiến trúc E/E

| Kiến trúc | Đặc điểm |
|---|---|
| **Distributed** | Mỗi chức năng 1 ECU, nhiều mạng CAN/LIN qua gateway |
| **Domain-based** | Domain controller cho Powertrain, Chassis, Body, Infotainment, ADAS; backbone Ethernet |
| **Zonal** | ECU theo vị trí vật lý (zone trước/sau/trái/phải) gom I/O, nối về **central compute (HPC)** qua Ethernet. Giảm dây điện, hỗ trợ **Software-Defined Vehicle (SDV)** |

Hệ quả cho firmware engineer:
- Zone ECU: nhiều I/O, driver, gateway signal ↔ Ethernet (SOME/IP), yêu cầu real-time và safety.
- HPC: SoC đa nhân, Linux/QNX/hypervisor, Adaptive AUTOSAR, kèm safety MCU (Cortex-R / TriCore) bên cạnh.

## 5. Vòng đời ECU

```
Requirement → Architecture → Design → Implementation → Unit test → Integration
→ HIL test → Vehicle test → SOP (Start of Production) → Aftersales (diag, reflash, OTA)
```
Các bên: **OEM** (hãng xe: Toyota, VinFast, BMW…), **Tier 1** (Bosch, Continental, Denso, ZF… làm ECU), **Tier 2** (NXP, Infineon, Renesas, ST… làm chip; Vector, Elektrobit… làm phần mềm/tool).

---

## Câu hỏi tự kiểm tra
1. Kể tên 5 ECU và chức năng.
2. Zonal architecture giải quyết vấn đề gì của kiến trúc distributed?
3. Vai trò của gateway ECU?
4. Phân biệt OEM, Tier 1, Tier 2.
