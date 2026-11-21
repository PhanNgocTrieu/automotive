# 03 – Điện tử số (Digital Logic)

## 1. Mức logic

| Chuẩn | VIL max | VIH min | VOL max | VOH min |
|---|---|---|---|---|
| CMOS 3.3V | 0.8 V (~0.3 VDD) | 2.0 V (~0.7 VDD) | 0.4 V | 2.4 V |
| CMOS 5V | 1.5 V | 3.5 V | 0.5 V | 4.4 V |

- Vùng giữa VIL và VIH là **vùng không xác định** → tránh.
- Nối MCU 3.3V với thiết bị 5V: kiểm tra chân có **5V tolerant (FT)** không; nếu không phải dùng level shifter.
- **Schmitt trigger input**: có hysteresis, chống nhiễu cho tín hiệu chậm/nhiễu.

## 2. Hệ đếm & biểu diễn số

```
Decimal 200 = Binary 1100_1000 = Hex 0xC8
```
- **Bit, nibble (4 bit), byte (8 bit), half-word (16), word (32)** – chú ý "word" phụ thuộc kiến trúc.
- **Số có dấu – bù 2 (two's complement):** `-x = ~x + 1`. int8_t: -128..127.
- **Endianness:**
  - Little-endian (ARM Cortex-M, x86, TriCore): byte thấp ở địa chỉ thấp.
  - Big-endian (Motorola, PowerPC, giao thức mạng, nhiều tín hiệu CAN "Motorola format").
  - Giá trị 0x12345678 tại địa chỉ 0x1000:
    - LE: `78 56 34 12`
    - BE: `12 34 56 78`
- **Fixed-point:** MCU không có FPU dùng số nguyên có scale. Ví dụ Q15, hoặc "physical = raw × factor + offset" như trong DBC.

## 3. Cổng logic & đại số Boole

AND, OR, NOT, NAND, NOR, XOR. Định lý De Morgan:
```
~(A & B) = ~A | ~B
~(A | B) = ~A & ~B
```
Ứng dụng trực tiếp vào bit manipulation trong C (xem `02-firmware/03-embedded-c.md`).

## 4. Mạch tuần tự

- **Latch / Flip-flop (D-FF):** lưu 1 bit, cập nhật theo cạnh clock.
- **Register:** nhóm flip-flop. Thanh ghi ngoại vi trong MCU (peripheral register) chính là các register được map vào địa chỉ bộ nhớ.
- **Counter / Timer:** bộ đếm theo clock → nền tảng của Timer/PWM peripheral.
- **Shift register:** nền tảng của UART/SPI.
- **Setup/hold time, metastability:** tín hiệu bất đồng bộ đi vào domain clock cần synchronizer (2 FF). Trong MCU điều này giải thích vì sao có "synchronization delay" khi đọc input.

## 5. Bộ nhớ

| Loại | Đặc điểm | Trong MCU |
|---|---|---|
| SRAM | Nhanh, mất khi mất điện | RAM data, stack |
| Flash NOR | Không mất, đọc nhanh, ghi theo page/sector, phải xoá trước khi ghi | Code (P-Flash), Data Flash (D-Flash) |
| EEPROM | Ghi theo byte, chu kỳ ghi nhiều hơn flash | Ít MCU còn có; thường dùng EEPROM emulation |
| ROM / OTP | Ghi một lần | Boot ROM, key |

## 6. Bus và giao tiếp

- **Song song vs nối tiếp.** Hầu hết giao tiếp ngoại vi hiện đại là nối tiếp.
- **Đồng bộ (có clock: SPI, I2C) vs bất đồng bộ (UART, CAN – đồng bộ bằng bit edge).**
- **Single-ended vs differential** (CAN, LVDS, Ethernet) – differential chống nhiễu common-mode rất tốt → lý do CAN dùng trên xe.

---

## Câu hỏi tự kiểm tra
1. Biểu diễn -5 dưới dạng int8_t bù 2 (hex)?
2. Giá trị uint16 0xABCD truyền big-endian, byte đầu tiên là gì?
3. Tại sao CAN dùng tín hiệu vi sai?
4. Hysteresis của Schmitt trigger giúp gì cho input từ công tắc cơ khí?

## Thực hành
- Viết hàm C chuyển đổi endianness 16/32-bit, kiểm tra bằng unit test.
- Mô phỏng D flip-flop & counter 4-bit trên **Logisim Evolution**.
