# 04 – Nguồn điện & Power Electronics

## 1. Hệ thống điện trên xe

| Hệ thống | Điện áp danh định | Dải hoạt động |
|---|---|---|
| Xe con 12V | 12 V (ắc quy chì) | ~9–16 V (cranking có thể xuống 3–6 V) |
| Xe tải 24V | 24 V | ~18–32 V |
| Mild hybrid 48V | 48 V | 36–52 V |
| EV high-voltage | 400 V / 800 V | Cần cách ly (isolation), an toàn HV |

Các ký hiệu terminal (DIN 72552):
- **KL30 (Terminal 30):** +Battery trực tiếp, luôn có điện.
- **KL15 (Terminal 15):** Ignition ON.
- **KL31:** Ground (mass).
- **KL50:** Crank/Start.

ECU thường có input KL15 làm **wake-up signal**, hoặc wake-up qua CAN/LIN.

## 2. Hiện tượng nguồn khắc nghiệt (ISO 16750-2, ISO 7637-2)

| Hiện tượng | Mô tả | Biện pháp |
|---|---|---|
| **Cranking / cold crank** | Khởi động máy, áp sụt xuống 3–6V vài trăm ms | Buck-boost, tụ bulk, brown-out handling trong firmware |
| **Load dump** | Ngắt ắc quy khi máy phát đang sạc → xung tới 40–100V+ | TVS, clamp, MOSFET bảo vệ |
| **Reverse battery** | Đấu ngược cực | Diode / ideal diode / P-MOSFET |
| **Jump start** | 24V trong vài phút | Linh kiện chịu áp ≥ 40V |
| **Transient pulses (ISO 7637)** | Xung từ tải cảm | TVS, lọc LC |

Firmware liên quan: giám sát áp nguồn (ADC), chuyển sang **degraded mode** khi under/over-voltage, không ghi Flash khi áp thấp, set DTC "Battery voltage low/high".

## 3. Linear Regulator (LDO)

```
Vin ──►[LDO]──► Vout     P_loss = (Vin - Vout) × Iout
```
- Ưu điểm: đơn giản, ít nhiễu.
- Nhược: tổn hao lớn khi chênh áp lớn. Ví dụ 14V → 5V @ 200mA: P = 1.8W → nóng!
- **Dropout voltage:** chênh áp tối thiểu để vẫn ổn áp.
- Automotive LDO: TLE4275, TPS7B69-Q1... có chân reset, watchdog tích hợp.

## 4. Switching Regulator (SMPS)

| Loại | Chức năng |
|---|---|
| Buck | Hạ áp (14V → 5V/3.3V), hiệu suất 85–95% |
| Boost | Tăng áp |
| Buck-Boost / SEPIC | Ra ổn định khi vào cao hơn hoặc thấp hơn (cranking) |
| Isolated (flyback, LLC) | Cách ly, dùng cho HV / EV |

Buck cơ bản:
```
Vin ──[SW]──┬──[L]──┬── Vout
            │       │
          [D/SW2]  [C]
            │       │
           GND     GND
D = Vout / Vin (duty cycle lý tưởng)
```
Nhược: gây nhiễu EMI → cần layout tốt, spread spectrum, tần số switching > 2 MHz (ngoài băng AM radio) là xu hướng automotive.

## 5. System Basis Chip (SBC) – Rất quan trọng với firmware

ECU thực tế thường dùng **SBC** (NXP FS65/UJA1169, Infineon TLE9xxx, TLF35584) tích hợp:
- Regulator (buck/LDO) cho MCU & ngoại vi
- CAN/LIN transceiver
- **Watchdog bên ngoài** (window watchdog) – firmware phải "kick" qua SPI đúng cửa sổ thời gian
- Wake-up inputs, quản lý chế độ sleep/standby
- Giám sát áp (under/over-voltage), **safe-state output** (fail-safe pin) cho ISO 26262

→ Firmware phải viết driver SPI cho SBC, xử lý init sequence, watchdog servicing, chuyển mode ngủ.

## 6. Power-up sequencing & Reset

- **Power-on reset (POR), brown-out reset (BOR/LVD):** MCU tự reset khi áp thấp.
- Một số MCU/FPGA cần thứ tự cấp nguồn (core trước I/O...).
- Firmware nên đọc **reset reason register** khi khởi động (POR, WDG, SW reset, BOR) và ghi log/DTC.

## 7. Tiêu thụ dòng & Quiescent current

Khi xe tắt máy, toàn bộ ECU phải có **dòng tĩnh rất thấp** (thường < 100 µA/ECU) để không cạn ắc quy sau vài tuần đỗ xe. Firmware phải:
- Đưa MCU vào deep sleep/standby.
- Tắt nguồn ngoại vi, đưa transceiver CAN vào sleep.
- Cấu hình chân I/O không gây dòng rò.

---

## Câu hỏi tự kiểm tra
1. Tính tổn hao của LDO 12V → 3.3V @ 100mA. Có nên dùng không?
2. Load dump là gì? Firmware có thể làm gì khi phát hiện overvoltage?
3. Window watchdog khác gì watchdog thường?
4. Tại sao quiescent current quan trọng với ECU trên xe?

## Thực hành
- Mô phỏng buck converter trong LTspice (dùng model LT8609 / LTC sẵn có).
- Dùng nguồn DC điều chỉnh, hạ áp cấp cho board từ 12V xuống 6V, quan sát hành vi reset/brown-out của MCU.
