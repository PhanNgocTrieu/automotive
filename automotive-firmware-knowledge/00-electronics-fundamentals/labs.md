# Labs – Điện tử cơ bản

Công cụ: breadboard, multimeter, oscilloscope (hoặc mô phỏng **Falstad** / **LTspice**), board MCU bất kỳ.

| # | Lab | Mục tiêu | Kết quả cần nộp (tự đánh giá) |
|---|---|---|---|
| E1 | LED + điện trở | Ohm, tính hạn dòng | Bảng tính R, đo I thực tế |
| E2 | Cầu phân áp đo 12V | Divider, scale ADC | Công thức chuyển đổi raw → V, sai số |
| E3 | Nút nhấn & bouncing | Pull-up, quan sát bounce | Ảnh scope bouncing (thường 1–10 ms) |
| E4 | RC low-pass | τ, tần số cắt | Đo đáp ứng bước, so với lý thuyết |
| E5 | MOSFET điều khiển tải 12V | Low-side switch, flyback | Ảnh scope Vds có/không có diode flyback |
| E6 | PWM điều khiển độ sáng/motor | Duty cycle, tần số | Bảng duty vs dòng tiêu thụ |
| E7 | NTC đọc nhiệt độ | ADC, lookup table | Bảng tra & nội suy, so với nhiệt kế |
| E8 | Đo dòng sleep | Low-power | Dòng run vs sleep (µA) |
| E9 | Logic analyzer decode UART/SPI/I2C | Protocol | Ảnh decode từ PulseView |
| E10 | Đọc schematic | Pin mapping | File pin_mapping.md cho board của bạn |

## Chi tiết E3 – Bouncing
1. Nút nhấn nối GPIO + pull-up 10k.
2. Scope trigger cạnh xuống, single-shot, timebase 1 ms/div.
3. Nhấn nhiều lần, ghi lại số cạnh nhiễu và thời gian bounce dài nhất.
4. Thêm tụ 100nF song song nút → so sánh.
5. Dùng kết quả để chọn thời gian debounce phần mềm (xem `04-application/03-signal-processing-control.md`).

## Chi tiết E5 – Flyback
1. Relay 12V hoặc solenoid, N-MOSFET logic level, gate qua R 100Ω, pull-down gate 100k.
2. **Không có diode** (chú ý: dùng MOSFET có avalanche rating hoặc thời gian ngắn): đo Vds khi tắt → thấy spike lớn.
3. Thêm diode 1N4007/Schottky → spike bị kẹp ~Vbat + 0.7V.
4. Nhận xét: thời gian nhả relay chậm hơn khi có diode. Giải pháp: diode + zener nối tiếp.
