# 06 – Motor Control & EV (Electric Vehicle)

## Vai trò công việc
**Chức danh:** Motor Control Engineer, Inverter SW Engineer, BMS Engineer, Power Electronics SW Engineer.

**Công việc hằng ngày:**
- Phát triển thuật toán điều khiển motor (FOC) cho traction inverter, EPS, bơm, quạt
- Phần mềm BMS: đo cell, cân bằng, ước lượng SOC/SOH, bảo vệ
- Phần mềm OBC, DC-DC, giao tiếp sạc
- Tune vòng điều khiển trên bàn thử (dyno) và trên xe

## Phù hợp nếu bạn
Thích điện tử công suất, lý thuyết điều khiển, toán và mô phỏng.

## Điều kiện tiên quyết
- [02-semiconductors](../../automotive-firmware-knowledge/00-electronics-fundamentals/02-semiconductors.md), [04-power-electronics](../../automotive-firmware-knowledge/00-electronics-fundamentals/04-power-electronics.md), [03-timer-pwm-adc](../../automotive-firmware-knowledge/03-drivers/03-timer-pwm-adc.md), [03-signal-processing-control](../../automotive-firmware-knowledge/04-application/03-signal-processing-control.md), [05-model-based-design](../../automotive-firmware-knowledge/04-application/05-model-based-design.md)
- Toán: số phức, ma trận, phương trình vi phân cơ bản

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | Lý thuyết điều khiển: hàm truyền, Bode, ổn định, PI rời rạc, anti-windup | 3 |
| 02 | Motor: DC, BLDC, PMSM, cảm ứng – mô hình toán | 2 |
| 03 | Inverter 3 pha, gate driver, dead-time, SVPWM, đo dòng shunt/Hall | 3 |
| 04 | FOC: Clarke/Park, vòng dòng & tốc độ, resolver/encoder, sensorless (observer) | 4 |
| 05 | Timer/ADC chuyên cho motor: GTM (AURIX), eTPU, đồng bộ PWM–ADC | 2 |
| 06 | BMS: đo cell (AFE), cân bằng, SOC (Coulomb counting, Kalman), SOH, bảo vệ | 4 |
| 07 | Sạc: OBC, DC-DC, ISO 15118, CCS, CHAdeMO; an toàn cao áp & cách ly | 3 |
| 08 | Safety cho inverter/BMS (ASIL C/D), safe torque off | 3 |

**Danh sách bài cần viết:**
- [ ] `01-control-theory.md`
- [ ] `02-motor-models.md`
- [ ] `03-inverter-svpwm.md`
- [ ] `04-foc.md`
- [ ] `05-motor-control-peripherals.md`
- [ ] `06-bms.md`
- [ ] `07-charging-obc-dcdc.md`
- [ ] `08-ev-safety.md`

## Tài liệu
- *Electric Motor Control* – Sang-Hoon Kim; *Vector Control of Three-Phase AC Machines* – Nguyen Phung Quang & J.-A. Dittrich
- *Battery Management Systems* (Vol. 1, 2) – Gregory Plett (có khoá Coursera miễn phí)
- Application note FOC của TI (InstaSPIN), ST (MCSDK), Infineon, NXP

## Công cụ
MATLAB/Simulink (Motor Control Blockset), PLECS, Python (scipy, control); bộ kit motor: TI LAUNCHXL + BOOSTXL-DRV8323, ST X-NUCLEO-IHM08M1, NXP MCSPTE1AK144.

## Dự án thực hành
1. **FOC cho BLDC/PMSM** trên kit motor: tự viết Clarke/Park/SVPWM, vòng dòng 10–20 kHz, vòng tốc độ.
2. **BMS mini** cho pack 4S Li-ion: AFE (BQ76920/LTC6811), cân bằng thụ động, SOC bằng Coulomb counting + hiệu chỉnh OCV, bảo vệ quá áp/quá dòng.
3. **Mô phỏng → firmware:** tune PI trên Simulink/Python, so sánh đáp ứng mô phỏng với thực tế.

## Mốc hoàn thành
- [ ] Giải thích và tự viết được FOC từ đầu
- [ ] Motor chạy ổn định, có bảo vệ quá dòng và dừng an toàn
- [ ] Hoàn thành dự án 1 và một trong hai dự án còn lại

> ⚠️ Làm việc với pin Li-ion và điện áp cao rất nguy hiểm. Bắt đầu với điện áp thấp (< 48V), có cầu chì và nguồn giới hạn dòng.

## Hướng tiếp theo
Kết hợp với [03 Functional Safety](../03-functional-safety/).
