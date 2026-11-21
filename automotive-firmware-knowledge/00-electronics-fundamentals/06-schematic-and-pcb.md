# 06 – Đọc Schematic, Datasheet, PCB & EMC

Firmware engineer **không cần thiết kế PCB**, nhưng **bắt buộc đọc được schematic và datasheet** để viết driver và debug phần cứng.

## 1. Đọc schematic

Checklist khi nhận schematic mới của ECU:
1. **Nguồn:** MCU được cấp mấy rail (VDD core, VDDIO, VDDA)? Từ regulator/SBC nào?
2. **Clock:** Thạch anh ngoài bao nhiêu MHz? (ảnh hưởng cấu hình PLL)
3. **Reset & Boot pins:** chân boot mode, mạch reset, ai điều khiển reset (SBC?).
4. **Debug:** JTAG/SWD connector ở đâu.
5. **Mapping I/O:** Lập bảng `Pin MCU ↔ Net name ↔ Chức năng ↔ Active high/low ↔ Pull-up/down`.
6. **Giao tiếp:** CAN transceiver nào, chân STB/EN, termination có không; SPI tới SBC/EEPROM...
7. **Analog:** cầu phân áp, hệ số scale, tụ lọc → để tính công thức chuyển đổi trong firmware.
8. **Output công suất:** high-side switch, H-bridge, chân diagnosis/current sense.

> Mẹo: Tạo file **pin_mapping.xlsx / .md** cho mỗi project — đây là tài liệu đầu vào cho cấu hình Port/Dio driver.

Ký hiệu thường gặp: net label, power flag, `NC` (not connected), `DNP` (do not populate), `/RESET` hoặc `RESET_N` (active low).

## 2. Đọc datasheet MCU / IC

Thứ tự đọc của một firmware engineer:
| Tài liệu | Nội dung |
|---|---|
| **Datasheet** | Pinout, đặc tính điện, timing, package |
| **Reference Manual (RM)** | Mô tả chi tiết từng peripheral & register — *tài liệu chính khi viết driver* |
| **Errata sheet** | Lỗi silicon đã biết & workaround — **luôn đọc!** |
| **Application notes** | Hướng dẫn sử dụng, ví dụ |
| **Safety manual** | Với MCU ASIL – yêu cầu firmware phải làm gì để đạt ASIL |
| **Core manual** (ARM ARM, TriCore Arch) | Kiến trúc CPU, exception, instruction |

Đọc một chương peripheral trong RM:
1. Block diagram & features
2. Functional description (mode hoạt động)
3. Initialization sequence (thường có sẵn từng bước)
4. Interrupts / DMA requests
5. Register map: địa chỉ base, offset, reset value, R/W/W1C...

Ký hiệu register: `RW` read/write, `RO` read-only, `W1C` write-1-to-clear (cờ ngắt hay dùng), `RC` read-to-clear.

## 3. PCB cơ bản

- Layers, via, trace width (phụ thuộc dòng), ground plane.
- Decoupling cap phải đặt sát chân nguồn IC.
- Đường CAN_H/CAN_L đi cặp vi sai.
- Test point: rất có ích cho debug firmware — yêu cầu HW team thêm test point cho UART debug, GPIO spare.

## 4. EMC (Electromagnetic Compatibility)

Ô tô có yêu cầu EMC rất nghiêm ngặt (CISPR 25, ISO 11452, ISO 10605 ESD).
- **Emission:** ECU không được phát nhiễu quá mức.
- **Immunity:** ECU phải hoạt động đúng khi bị nhiễu.

Firmware ảnh hưởng EMC:
- Slew rate của GPIO (cấu hình drive strength thấp nếu không cần nhanh).
- Spread-spectrum clock (FM-PLL).
- Tần số PWM, tránh băng tần radio.
- Robustness: debounce, plausibility check, khôi phục sau khi bị nhiễu (re-init peripheral, watchdog).

---

## Câu hỏi tự kiểm tra
1. Khi viết driver, bạn đọc Datasheet hay Reference Manual? Tại sao phải đọc Errata?
2. Một cờ ngắt có thuộc tính W1C, viết code clear cờ như thế nào? Tại sao không dùng `REG |= FLAG`?
3. Liệt kê 5 thông tin phải lấy từ schematic trước khi cấu hình MCU.

## Thực hành
- Tải schematic của board bạn đang có (Nucleo / S32K144EVB). Lập bảng pin mapping cho LED, nút nhấn, UART debug, CAN.
- Mở Reference Manual, đọc chương GPIO và tìm: base address, register set/reset bit nguyên tử (BSRR / PSOR...).
