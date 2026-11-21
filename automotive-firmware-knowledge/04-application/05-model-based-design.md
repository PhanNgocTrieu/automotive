# 05 – Model-Based Design (MBD)

## 1. Khái niệm
Thiết kế thuật toán bằng **mô hình** (Simulink/Stateflow), mô phỏng, rồi **tự sinh code C** (Embedded Coder, TargetLink) đưa vào ECU. Rất phổ biến cho powertrain, chassis, BMS, motor control.

```
Requirement → Model (Simulink) → MIL test → Code gen → SIL test → PIL test → Tích hợp ECU → HIL → Vehicle
```

| Mức | Chạy gì | Ở đâu | Mục đích |
|---|---|---|---|
| **MIL** (Model-in-the-Loop) | Model | PC (Simulink) | Kiểm tra thuật toán |
| **SIL** (Software-in-the-Loop) | Code generate | PC | So sánh code vs model (back-to-back) |
| **PIL** (Processor-in-the-Loop) | Code | MCU thật | Kiểm tra compiler, fixed-point, timing |
| **HIL** (Hardware-in-the-Loop) | ECU thật | Simulator real-time | Kiểm thử hệ thống, fault injection |

## 2. Công cụ
- **MATLAB/Simulink, Stateflow, Embedded Coder, Simulink Test, Simulink Design Verifier, Polyspace**.
- **dSPACE TargetLink** (code gen), **dSPACE SCALEXIO** (HIL), **ASCET (ETAS)**.
- Thay thế miễn phí để học: **Scilab/Xcos**, **OpenModelica**, Python (scipy.signal, control).

## 3. Quy tắc mô hình
- **MAAB / JMAAB guidelines** (style guide cho Simulink).
- Fixed-step solver, kiểu dữ liệu rõ ràng (fixed-point nếu MCU không FPU).
- Model Advisor check, MISRA-compliant code gen.
- Interface model ↔ code tay rõ ràng (input/output, calibration, measurement).

## 4. Tích hợp code generate vào ECU
- Model generate hàm `Model_initialize()` và `Model_step()` → gọi trong task chu kỳ tương ứng với sample time.
- Với AUTOSAR: Simulink generate trực tiếp **SWC + ARXML** (runnable = step function).
- Calibration parameter → biến trong vùng `.calib` (A2L file cho INCA/CANape).

```c
/* Tích hợp tay */
void Task_10ms(void)
{
    rtU.CoolantTemp_dC = Rte_Read_CoolantTemp();
    rtU.Vbat_mV        = Rte_Read_Vbat();
    FanCtrl_step();
    Rte_Write_FanDuty(rtY.FanDuty_pct);
}
```

## 5. A2L & XCP
- **A2L (ASAM MCD-2 MC):** mô tả địa chỉ, kiểu, scaling của biến đo (measurement) và tham số chỉnh (characteristic).
- **XCP:** giao thức đọc/ghi RAM ECU thời gian thực qua CAN/Ethernet → calibration engineer chỉnh tham số trên xe.

## 6. Khi nào viết tay, khi nào dùng MBD?
| Viết tay | MBD |
|---|---|
| Driver, BSW, giao thức, state machine đơn giản | Thuật toán điều khiển, lọc, bảng tra phức tạp |
| Cần tối ưu tài nguyên tối đa | Cần mô phỏng, tune nhiều, team control engineer |

---

## Thực hành
- Mô hình hoá motor DC (phương trình điện + cơ) và PID trong Simulink hoặc Python (`scipy`), tune PID trên mô hình, sau đó đưa thông số vào firmware (lab PID ở bài 03) và so sánh đáp ứng thực tế với mô phỏng.
