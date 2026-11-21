# 04 – AUTOSAR

**AUTOSAR (AUTomotive Open System ARchitecture)**: chuẩn kiến trúc phần mềm cho ECU, giúp tái sử dụng phần mềm giữa các hãng/chip, tách application khỏi phần cứng.

## 1. Classic Platform (CP) vs Adaptive Platform (AP)

| | Classic | Adaptive |
|---|---|---|
| Phần cứng | MCU (Cortex-M/R, TriCore, RH850) | SoC/MPU (Cortex-A) |
| OS | AUTOSAR OS (dựa OSEK), static | POSIX (Linux, QNX) |
| Cấu hình | Static, compile-time | Dynamic, service-oriented |
| Ngôn ngữ | C | C++14/17 |
| Giao tiếp | Signal-based (CAN, LIN, FlexRay) | Service-based (SOME/IP, DDS) |
| Ứng dụng | Body, powertrain, chassis, safety | ADAS, HPC, infotainment, OTA |

## 2. Kiến trúc phân lớp Classic AUTOSAR

```
┌────────────────────────────────────────────────────────────────┐
│                  Application Layer (SWC)                       │
│     SWC_A          SWC_B           SWC_C   (Runnables)         │
├────────────────────────────────────────────────────────────────┤
│                RTE (Runtime Environment) – generated          │
├──────────────┬──────────────┬──────────────┬───────────────────┤
│   Services   │   Services   │   Services   │                   │
│ OS, EcuM,BswM│ Com,PduR,Nm  │ Dcm,Dem,NvM  │  Complex Device   │
│  WdgM, Det   │  CanSM,CanTp │  FiM, Crypto │  Drivers (CDD)    │
├──────────────┼──────────────┼──────────────┤                   │
│  ECU Abstr.  │ CanIf, LinIf │ MemIf, Fee/Ea│  (truy cập HW     │
│  IoHwAb      │ CanTrcv      │ WdgIf        │   trực tiếp)      │
├──────────────┼──────────────┼──────────────┤                   │
│ MCAL: Mcu, Port, Dio, Adc, Pwm, Icu, Gpt, Spi, Can, Lin, Fls, Wdg│
├────────────────────────────────────────────────────────────────┤
│                        Microcontroller                         │
└────────────────────────────────────────────────────────────────┘
```

### Các module quan trọng
| Module | Chức năng |
|---|---|
| **Mcu** | Clock, PLL, reset, low-power mode |
| **Port / Dio** | Cấu hình chân / đọc ghi digital |
| **Adc, Pwm, Icu, Gpt** | Analog, PWM, input capture, timer |
| **Spi, Can, Lin, Eth, Fls, Wdg** | Driver ngoại vi |
| **CanIf** | Abstraction giữa CAN driver và tầng trên |
| **PduR** | Router PDU giữa Com, Dcm, CanTp... |
| **Com** | Pack/unpack signal ↔ I-PDU, timeout, chu kỳ gửi |
| **CanTp** | ISO-TP |
| **CanSM / ComM / CanNm** | State manager, communication manager, network management |
| **Dcm** | Diagnostic Communication Manager – xử lý UDS |
| **Dem** | Diagnostic Event Manager – quản lý DTC |
| **FiM** | Function Inhibition Manager – chặn chức năng khi có lỗi |
| **NvM / MemIf / Fee / Ea** | Quản lý bộ nhớ non-volatile |
| **EcuM / BswM** | Khởi động/tắt ECU, mode management |
| **WdgM** | Watchdog manager: alive, deadline, logical supervision |
| **Det** | Default Error Tracer – báo lỗi phát triển |
| **Os** | AUTOSAR OS |
| **Crypto stack, SecOC** | Bảo mật |

## 3. SWC, Port, Interface, Runnable

- **SWC (Software Component):** đơn vị chức năng application. Loại: Application, Sensor/Actuator, Composition, Service...
- **Port:** điểm giao tiếp – **P-Port** (provide), **R-Port** (require).
- **Interface:**
  - **Sender-Receiver (S/R):** gửi dữ liệu (signal).
  - **Client-Server (C/S):** gọi hàm (operation).
  - Mode switch, Parameter, NvData, Trigger.
- **Runnable:** hàm C được RTE gọi theo **RTE event** (timing 10ms, data received, operation invoked…).
- **VFB (Virtual Functional Bus):** khái niệm SWC giao tiếp không cần biết chúng ở cùng ECU hay khác ECU.

Code ví dụ (do RTE generate API):
```c
/* Runnable chạy mỗi 10 ms */
void HeadlightCtrl_Run10ms(void)
{
    uint8 lightSwitch;
    uint16 ambientLux;

    (void)Rte_Read_RP_LightSwitch_Value(&lightSwitch);
    (void)Rte_Read_RP_AmbientLight_Lux(&ambientLux);

    boolean on = (lightSwitch == LIGHT_SW_ON) ||
                 ((lightSwitch == LIGHT_SW_AUTO) && (ambientLux < 200u));

    (void)Rte_Write_PP_HeadlightCmd_Value(on);
}
```

## 4. Luồng dữ liệu nhận một CAN signal
```
CAN bus → Can (MCAL, ISR/polling) → CanIf (Rx indication) → PduR → Com (unpack signal)
→ RTE → SWC Rte_Read()
```
Luồng diag: `Can → CanIf → PduR → CanTp → PduR → Dcm`.

## 5. Methodology & Tool

```
System description (ARXML) → ECU extract → ECU configuration (BSW + RTE)
→ Code generation → Build cùng application code → Flash
```
- Mọi thứ mô tả bằng **ARXML**.
- Tool: **Vector DaVinci Developer/Configurator + MICROSAR**, **Elektrobit EB tresos (AutoCore)**, **ETAS RTA-CAR/ISOLAR**, **NXP S32 Design Studio + RTD (MCAL miễn phí cho S32K)**, **Mentor/Siemens Capital VSTAR**.
- Có open-source để học: **Arctic Core (cũ)**, **openAUTOSAR**, các project trên GitHub mô phỏng AUTOSAR stack.

## 6. Adaptive AUTOSAR (tóm tắt)
- **ara::com** (service communication), **ara::exec** (execution management), **ara::diag**, **ara::per** (persistency), **ara::log**, **ara::phm** (platform health), **UCM** (update & config management – OTA).
- Ứng dụng là các process POSIX, deploy qua manifest.

---

## Câu hỏi tự kiểm tra
1. Vai trò của RTE? Tại sao SWC không gọi trực tiếp Com?
2. Phân biệt S/R và C/S interface. Cho ví dụ.
3. Vẽ đường đi của một UDS request từ CAN đến Dcm.
4. Khi nào cần Complex Device Driver?
5. Dem và Dcm khác nhau thế nào?

## Thực hành
- Cài **NXP S32 Design Studio** + **S32K1 RTD**, tạo project với MCAL Dio/Port, blink LED bằng `Dio_WriteChannel()`.
- Tự viết một "mini-RTE" trong project capstone: các SWC chỉ giao tiếp qua hàm `Rte_Read_xxx/Rte_Write_xxx`.
