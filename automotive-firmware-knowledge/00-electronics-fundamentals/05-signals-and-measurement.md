# 05 – Tín hiệu & Đo lường

## 1. Analog vs Digital

- **Analog:** liên tục (cảm biến nhiệt NTC, vị trí bàn đạp ga, áp suất).
- **Digital:** rời rạc 0/1 (công tắc, tín hiệu PWM, bus truyền thông).

## 2. ADC (Analog-to-Digital Converter)

```
Digital = Vin / Vref × (2^N - 1)
LSB     = Vref / 2^N
```
Ví dụ: ADC 12-bit, Vref = 3.3V → LSB ≈ 0.806 mV.

| Khái niệm | Giải thích |
|---|---|
| Resolution | Số bit (10, 12, 16) |
| Sampling rate | Số mẫu/giây |
| **Nyquist** | f_sample ≥ 2 × f_max tín hiệu, thực tế 5–10× |
| Aliasing | Tín hiệu tần số cao "giả" thành tần số thấp nếu không lọc → cần anti-aliasing RC filter |
| Sample & hold time | Thời gian nạp tụ lấy mẫu, phụ thuộc trở kháng nguồn |
| INL/DNL, offset, gain error | Sai số – có thể cần calibration |
| Ratiometric measurement | Cảm biến cấp nguồn cùng Vref với ADC → sai số nguồn tự triệt tiêu |

**Loại ADC:** SAR (phổ biến nhất trong MCU), Sigma-Delta (độ phân giải cao, chậm), Flash (rất nhanh).

### Ví dụ: Đọc cảm biến nhiệt NTC
```
 Vref ──[R_fixed 10k]──┬── ADC
                       │
                     [NTC]
                       │
                      GND
R_ntc = R_fixed × adc / (ADC_MAX - adc)
T(K)  = 1 / (1/T0 + ln(R_ntc/R0)/B)     // phương trình Beta
```
Trên MCU nhỏ thường dùng **lookup table + nội suy tuyến tính** thay vì tính log.

## 3. DAC
Chuyển số → áp. Ít dùng trong ECU; thay thế phổ biến là **PWM + RC filter**.

## 4. PWM (Pulse Width Modulation)

```
 ┌──┐    ┌──┐    ┌──┐
 │  │    │  │    │  │
─┘  └────┘  └────┘  └──
 |Ton|
 |-- T --|
Duty = Ton / T     f = 1/T
```
Ứng dụng: điều khiển độ sáng đèn, tốc độ motor, quạt, van solenoid, cuộn kích máy phát. Cảm biến cũng có thể *xuất* PWM (cảm biến vị trí, cảm biến dòng) → MCU đo bằng **Input Capture**.

Lưu ý: tần số PWM ảnh hưởng tiếng ồn nghe được (> 20 kHz cho motor), tổn hao switching, EMI.

## 5. Nhiễu & Toàn vẹn tín hiệu

- **Nguồn nhiễu:** motor, ignition, switching regulator, relay.
- **Biện pháp phần cứng:** lọc RC/LC, dây xoắn đôi, shielding, ground plane, tách analog/digital ground.
- **Biện pháp phần mềm:** oversampling + trung bình, median filter, debounce, kiểm tra plausibility (giá trị hợp lý?).
- **Ringing / reflection:** đường truyền dài cần termination (CAN: 120Ω hai đầu bus).

## 6. Dụng cụ đo

| Dụng cụ | Dùng để |
|---|---|
| **Multimeter** | Đo V/I/R, thông mạch, diode |
| **Oscilloscope** | Xem dạng sóng theo thời gian: PWM, ringing, noise, timing ISR (toggle GPIO) |
| **Logic analyzer** | Nhiều kênh digital, decode UART/SPI/I2C/CAN – cực hữu ích cho firmware |
| **CAN analyzer** (PCAN, Vector CANoe, Kvaser) | Ghi/gửi frame CAN, decode bằng DBC |
| **Power analyzer / µCurrent** | Đo dòng tiêu thụ sleep mode |
| **Debugger (J-Link, ST-Link, Lauterbach)** | Debug firmware |

### Kỹ thuật oscilloscope quan trọng
- Trigger cạnh (edge), trigger độ rộng xung, single-shot.
- Đo với **ground spring** ngắn để tránh nhiễu.
- **Kỹ thuật "GPIO toggle":** set GPIO HIGH đầu ISR, LOW cuối ISR → đo thời gian thực thi và jitter bằng scope. Đây là kỹ thuật debug timing rất phổ biến của firmware engineer.

## 7. Cảm biến thường gặp trên xe

| Cảm biến | Ngõ ra | Giao tiếp với MCU |
|---|---|---|
| Nhiệt độ (NTC) | Điện trở | ADC |
| Vị trí bàn đạp ga (APP) | 2 kênh analog dư thừa | ADC ×2 (safety: so sánh chéo) |
| Tốc độ bánh xe (ABS) | Xung / dòng (Hall, AMR) | Input capture / chuyên dụng |
| Áp suất | Analog / SENT | ADC / SENT decoder |
| Vị trí trục cam/khuỷu | Xung Hall/VR | Timer capture |
| IMU / gia tốc | Digital | SPI |
| Hall vị trí rotor motor | Digital 3 kênh | GPIO/Timer |

**SENT (SAE J2716)** và **PSI5** là giao thức cảm biến đặc thù automotive.

---

## Câu hỏi tự kiểm tra
1. ADC 10-bit, Vref = 5V, đọc được 512. Điện áp vào bao nhiêu?
2. Tín hiệu 1 kHz, lấy mẫu 1.2 kHz → điều gì xảy ra?
3. Tại sao cảm biến bàn đạp ga dùng 2 kênh?
4. Làm sao đo thời gian thực thi một ISR bằng oscilloscope?

## Thực hành
- Đọc biến trở bằng ADC, so sánh giá trị raw khi có/không có tụ lọc 100nF.
- Tạo PWM 1 kHz từ MCU, đo duty bằng scope và bằng input capture trên MCU thứ 2 (hoặc kênh timer khác).
