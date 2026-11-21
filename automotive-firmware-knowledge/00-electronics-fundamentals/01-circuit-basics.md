# 01 – Mạch điện cơ bản

## 1. Các đại lượng cơ bản

| Đại lượng | Ký hiệu | Đơn vị | Ý nghĩa |
|---|---|---|---|
| Điện áp | V, U | Volt (V) | Chênh lệch thế năng giữa 2 điểm |
| Dòng điện | I | Ampere (A) | Lượng điện tích chạy qua trong 1 giây |
| Điện trở | R | Ohm (Ω) | Cản trở dòng điện |
| Công suất | P | Watt (W) | P = V × I = I²R = V²/R |
| Điện dung | C | Farad (F) | Khả năng tích điện |
| Điện cảm | L | Henry (H) | Chống lại sự thay đổi dòng điện |

**Quy ước:** Điện áp luôn đo *giữa hai điểm*. Khi nói "chân này 3.3V" nghĩa là so với GND.

## 2. Định luật Ohm & Kirchhoff

- **Ohm:** `V = I × R`
- **KCL (định luật dòng):** Tổng dòng vào một nút = tổng dòng ra.
- **KVL (định luật áp):** Tổng điện áp trong một vòng kín = 0.

### Ví dụ: Tính điện trở hạn dòng cho LED
MCU xuất 3.3V, LED đỏ có Vf ≈ 2.0V, muốn I = 5mA:

```
R = (Vcc - Vf) / I = (3.3 - 2.0) / 0.005 = 260 Ω  → chọn 270 Ω (giá trị chuẩn E12)
P_R = I² × R = 0.005² × 270 = 6.75 mW → điện trở 0603 (1/10W) là đủ
```

> Lưu ý: chân GPIO của MCU thường chỉ chịu 4–25 mA. Luôn kiểm tra *datasheet → Absolute Maximum Ratings* và *DC characteristics*.

## 3. Mắc nối tiếp / song song

| | Nối tiếp | Song song |
|---|---|---|
| Điện trở | R = R1 + R2 | 1/R = 1/R1 + 1/R2 |
| Tụ điện | 1/C = 1/C1 + 1/C2 | C = C1 + C2 |
| Cuộn cảm | L = L1 + L2 | 1/L = 1/L1 + 1/L2 |

## 4. Cầu phân áp (Voltage Divider)

```
 Vin ──[R1]──┬── Vout
             │
            [R2]
             │
            GND
Vout = Vin × R2 / (R1 + R2)
```

**Ứng dụng automotive:** Đo điện áp ắc quy 12V (thực tế 9–16V, có thể lên 40V khi load dump) bằng ADC 3.3V/5V.

Ví dụ: Vbat max đo được = 20V, ADC ref = 3.3V → tỉ lệ ≈ 1/6.
Chọn R1 = 47kΩ, R2 = 10kΩ → tỉ lệ = 10/57 = 0.175 → 20V → 3.5V (hơi vượt!). Chọn R1 = 51kΩ → 20V → 3.28V ✓.

Thêm tụ 100nF song song R2 để lọc nhiễu, và diode/zener kẹp bảo vệ chân ADC.

**Trở kháng nguồn:** ADC SAR cần nguồn trở kháng thấp (thường < 10kΩ hoặc theo datasheet). Cầu phân áp trở lớn → cần tụ hold lớn hoặc buffer op-amp, hoặc tăng sample time.

## 5. Điện trở kéo lên / kéo xuống (Pull-up / Pull-down)

Một chân input "thả nổi" (floating) sẽ đọc ra giá trị ngẫu nhiên. Pull-up/pull-down đặt trạng thái mặc định.

```
 3.3V                           3.3V
  │                               │
 [10k] pull-up               (MCU pin)──┬──[Switch]── 3.3V
  │                                     │
  ├── MCU pin                         [10k] pull-down
  │                                     │
[Switch]                               GND
  │
 GND
 Nhấn → 0 (active low)           Nhấn → 1 (active high)
```

- Giá trị điển hình: 4.7kΩ – 100kΩ. Nhỏ → chống nhiễu tốt nhưng tốn dòng.
- I2C dùng pull-up 2.2k–10k (open-drain bus).
- Trên xe, input từ công tắc thường là "switch to ground" hoặc "switch to battery" với dòng "wetting current" vài mA để làm sạch tiếp điểm.

## 6. Tụ điện (Capacitor)

- `Q = C × V`, `I = C × dV/dt`
- Không cho DC đi qua (ở trạng thái xác lập), cho AC đi qua.
- **Ứng dụng:** lọc nguồn, decoupling (100nF sát chân VDD của IC), tích trữ năng lượng, tạo thời gian (RC), coupling tín hiệu.

Loại tụ:
| Loại | Đặc điểm | Dùng cho |
|---|---|---|
| Ceramic MLCC (X7R, C0G) | Nhỏ, ESR thấp, giá trị giảm theo DC bias | Decoupling, lọc HF |
| Electrolytic nhôm | Giá trị lớn, ESR cao, tuổi thọ phụ thuộc nhiệt | Bulk capacitor nguồn |
| Tantalum / Polymer | Ổn định, ESR thấp | Nguồn, nhưng tantalum có thể cháy khi quá áp |

Automotive dùng linh kiện chuẩn **AEC-Q200** (thụ động) và **AEC-Q100** (IC).

## 7. Mạch RC – Hằng số thời gian

```
τ = R × C
Nạp:  Vc(t) = Vin × (1 - e^(-t/τ))   → sau 5τ ≈ 99.3%
Xả:   Vc(t) = V0 × e^(-t/τ)
```

**Lọc thông thấp RC (Low-pass filter):**
```
f_c = 1 / (2π × R × C)
```
Ví dụ R = 10kΩ, C = 100nF → fc ≈ 159 Hz. Dùng trước ADC để chống aliasing (anti-aliasing filter).

## 8. Cuộn cảm (Inductor)

- `V = L × dI/dt` → chống thay đổi dòng đột ngột.
- Khi ngắt dòng qua cuộn cảm (relay coil, solenoid, motor) → điện áp ngược rất lớn (inductive kickback) → **bắt buộc dùng diode freewheeling (flyback)** hoặc TVS/clamp.
- Ứng dụng: Buck/Boost converter, ferrite bead lọc EMI, common-mode choke trên CAN bus.

## 9. AC cơ bản

- Trở kháng tụ: `Xc = 1 / (2πfC)` – tần số cao → trở kháng nhỏ.
- Trở kháng cuộn cảm: `XL = 2πfL` – tần số cao → trở kháng lớn.
- RMS của sóng sin: `Vrms = Vpeak / √2`.
- Duty cycle của PWM: `Vavg = Vhigh × D`.

---

## Câu hỏi tự kiểm tra
1. Tại sao input floating là nguy hiểm? Nêu 2 cách xử lý.
2. Tính cầu phân áp để đo Vbat từ 0–32V bằng ADC 5V.
3. Tại sao cần tụ 100nF đặt sát chân VDD của MCU?
4. Điều gì xảy ra khi MCU ngắt relay mà không có diode flyback?
5. Một RC low-pass với R = 1kΩ, C = 1µF có tần số cắt bao nhiêu?

## Thực hành
- Mô phỏng các mạch trên bằng **Falstad Circuit Simulator** (online) hoặc **LTspice**.
- Breadboard: LED + điện trở, đo dòng bằng multimeter; nút nhấn với pull-up, quan sát bouncing bằng oscilloscope.
