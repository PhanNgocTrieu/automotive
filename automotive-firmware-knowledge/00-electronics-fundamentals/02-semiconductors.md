# 02 – Linh kiện bán dẫn

## 1. Diode

| Loại | Vf điển hình | Ứng dụng |
|---|---|---|
| Diode chỉnh lưu (1N4007) | 0.7–1.0 V | Chỉnh lưu, bảo vệ ngược cực |
| Schottky (SS34, BAT54) | 0.2–0.45 V | Nguồn, flyback, OR-ing, tổn hao thấp |
| Zener | Vz cố định (3.3V, 5.1V...) | Kẹp áp, tham chiếu áp đơn giản |
| TVS (SMBJ, SMCJ) | Kẹp xung nhanh | Bảo vệ ESD, load dump, xung quá áp |
| LED | 1.8–3.3 V | Hiển thị |

### Bảo vệ ngược cực (Reverse polarity protection)
Trên xe, ECU phải chịu được việc đấu ngược ắc quy (thường -14V trong 60s theo ISO 16750-2).
- Cách đơn giản: diode nối tiếp (tổn hao P = Vf × I).
- Cách tốt hơn: **P-MOSFET** ở high-side hoặc **ideal diode controller** (LM74700-Q1) + N-MOSFET → tổn hao rất nhỏ.

### Diode flyback
```
   +12V
    │
  ┌─┴─┐
  │Coil│◄── Relay / solenoid
  └─┬─┘   ▲ Diode (cathode lên +12V)
    │─────┘
    │
  Drain
  [N-MOSFET] ← Gate từ MCU
  Source
    │
   GND
```

## 2. BJT (Bipolar Junction Transistor)

- Điều khiển bằng **dòng** base: `Ic = β × Ib` (vùng khuếch đại).
- Dùng làm switch: đưa vào vùng bão hoà (Vce(sat) ≈ 0.2V).
- NPN low-side switch: tính `Rb = (V_GPIO - 0.7) / (Ic/β × hệ số dư 2–5)`.

Ngày nay trong ECU, BJT ít dùng làm switch công suất, chủ yếu thay bằng MOSFET hoặc **Smart High-Side Switch**.

## 3. MOSFET

- Điều khiển bằng **điện áp** Vgs. Gate gần như không ăn dòng DC (nhưng có điện dung Ciss cần nạp/xả).
- **N-MOSFET**: thường dùng low-side (Source nối GND). Rds(on) nhỏ.
- **P-MOSFET**: dùng high-side đơn giản (Source nối +V).

Thông số quan trọng khi chọn:
| Thông số | Ý nghĩa |
|---|---|
| Vds(max) | Áp tối đa D-S, xe 12V nên chọn ≥ 40V |
| Id | Dòng liên tục |
| Rds(on) @ Vgs | Điện trở dẫn – xem ở đúng Vgs mà MCU cấp (3.3V → cần "logic-level" MOSFET) |
| Vgs(th) | Ngưỡng bắt đầu dẫn – **không phải** ngưỡng dẫn hoàn toàn |
| Qg | Điện tích gate – ảnh hưởng tốc độ switch PWM |

### Gate driver
Khi PWM tần số cao (motor, buck converter) cần **gate driver** để nạp/xả gate nhanh, giảm tổn hao chuyển mạch. Với cầu H (H-bridge) cho motor DC dùng IC như DRV8873-Q1, hoặc 3-pha cho BLDC.

## 4. Smart Power Switch (rất phổ biến trong automotive)

IC như **Infineon PROFET (BTS7xxx)**, **ST VIPower (VN7xxx)**:
- High-side switch tích hợp MOSFET + bảo vệ quá dòng, quá nhiệt, short to GND/BAT.
- Có chân **current sense (IS)** → MCU đọc ADC để chẩn đoán: open load, short circuit.
- Firmware dùng nó để làm **diagnostic output** (ví dụ: phát hiện bóng đèn bị cháy → DTC).

## 5. Op-amp (Khuếch đại thuật toán)

Mạch cơ bản:
| Mạch | Công thức |
|---|---|
| Buffer (voltage follower) | Vout = Vin |
| Khuếch đại không đảo | Vout = Vin × (1 + Rf/Rg) |
| Khuếch đại đảo | Vout = -Vin × Rf/Rin |
| Comparator | Vout = High nếu V+ > V- |
| Khuếch đại vi sai / current sense | Đo dòng qua shunt resistor |

**Đo dòng bằng shunt:** R_shunt = 10mΩ, I = 10A → V = 100mV → khuếch đại ×20 → 2V vào ADC. Thường dùng IC current-sense amplifier chuyên dụng (INA240-Q1).

## 6. Transistor trong MCU – Cấu hình chân output

- **Push-pull:** chủ động kéo lên VDD và xuống GND.
- **Open-drain:** chỉ kéo xuống GND, cần pull-up ngoài → dùng cho I2C, wired-AND, đổi mức logic.

---

## Câu hỏi tự kiểm tra
1. Tại sao MOSFET có Vgs(th) = 2V vẫn có thể không dẫn tốt khi Vgs = 3.3V?
2. So sánh N-MOSFET low-side và P-MOSFET high-side. Trên xe, tại sao tải thường được switch ở high-side?
3. Smart high-side switch hỗ trợ firmware chẩn đoán lỗi tải như thế nào?
4. Vẽ mạch bảo vệ ngược cực dùng P-MOSFET.

## Thực hành
- Điều khiển một motor DC nhỏ 12V bằng N-MOSFET logic-level (IRLZ44N / AO3400) + diode flyback từ GPIO, PWM tốc độ.
- Đo Vds khi MOSFET dẫn để tính Rds(on) thực tế.
