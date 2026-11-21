# 06 – Bộ nhớ & Non-Volatile Memory (NvM)

## 1. Cần lưu gì vào bộ nhớ không mất dữ liệu?
- DTC + freeze frame + counters.
- Odometer, giờ hoạt động.
- Cấu hình xe (variant coding), VIN.
- Calibration học được (adaptive values: vị trí bướm ga, offset cảm biến).
- Trạng thái lần tắt máy trước (đèn, ghế nhớ vị trí).
- Key / security counter.

## 2. Loại bộ nhớ
| Loại | Ưu điểm | Nhược điểm |
|---|---|---|
| D-Flash nội (data flash) | Có sẵn, rẻ | Phải erase sector, giới hạn chu kỳ (10k–100k) |
| EEPROM ngoài (SPI/I2C) | Ghi theo byte, 1M chu kỳ | Thêm linh kiện, giao tiếp chậm |
| FRAM/MRAM | Ghi nhanh, gần như vô hạn | Đắt |
| Emulated EEPROM (FlexNVM trên S32K) | MCU hỗ trợ phần cứng | |

## 3. EEPROM emulation trên Flash (Fee)
Ý tưởng: không ghi đè tại chỗ, mà **append** bản ghi mới vào sector đang hoạt động; khi đầy thì **copy bản ghi mới nhất** sang sector khác rồi xoá sector cũ (swap / garbage collection).

```
Sector A (active)                         Sector B (erased)
┌──────────────────────────────┐
│ Header: ACTIVE, counter=5    │
│ [ID=1, len, data, CRC]       │
│ [ID=2, len, data, CRC]       │
│ [ID=1, len, data', CRC] ←mới │
│ ... free (0xFF) ...          │
└──────────────────────────────┘
Khi đầy → copy bản mới nhất mỗi ID sang B → đánh dấu B ACTIVE → erase A
```
Phải xử lý **mất điện giữa chừng**: mỗi bước có trạng thái đánh dấu, khi khởi động kiểm tra và khôi phục.

**Wear leveling:** phân tán số lần ghi để kéo dài tuổi thọ. Tính toán:
```
Tuổi thọ = (Số sector × Kích thước sector / Kích thước bản ghi) × Chu kỳ erase / tần suất ghi
```

## 4. AUTOSAR Memory stack
```
SWC / Dem ──► NvM ──► MemIf ──┬─► Fee ──► Fls (flash driver)
                               └─► Ea  ──► Eep (EEPROM driver)
```
- **NvM block:** RAM mirror + ROM default + NV storage.
- Loại block: **Native**, **Redundant** (2 bản sao – dùng cho dữ liệu quan trọng), **Dataset** (nhiều phiên bản).
- **NvM_ReadAll()** lúc khởi động, **NvM_WriteAll()** lúc shutdown (sau khi tắt ignition, ECU giữ nguồn qua SBC để ghi xong rồi mới tắt).
- Ghi bất đồng bộ (`NvM_WriteBlock` → job xử lý trong `NvM_MainFunction`).
- CRC mỗi block để phát hiện hỏng → dùng default value nếu hỏng.

## 5. ECC & toàn vẹn dữ liệu
- **ECC (SEC-DED):** sửa 1 bit, phát hiện 2 bit lỗi. Lỗi 2 bit → exception/BusFault → firmware phải xử lý (ví dụ khi đọc vùng D-Flash bị mất điện lúc ghi).
- **CRC:** CRC8 (SAE J1850 cho E2E), CRC16-CCITT, CRC32 (Ethernet poly) cho image/block.

```c
/* CRC-8 SAE J1850 (poly 0x1D, init 0xFF, xorout 0xFF) – bitwise, dễ hiểu */
uint8_t crc8_j1850(const uint8_t *d, uint32_t n)
{
    uint8_t crc = 0xFFu;
    for (uint32_t i = 0u; i < n; i++) {
        crc ^= d[i];
        for (uint8_t b = 0u; b < 8u; b++) {
            crc = ((crc & 0x80u) != 0u) ? (uint8_t)((crc << 1) ^ 0x1Du) : (uint8_t)(crc << 1);
        }
    }
    return (uint8_t)(crc ^ 0xFFu);
}
```
Production nên dùng **table-driven** hoặc **CRC hardware** của MCU.

## 6. Stack & RAM
- Tính stack: phân tích tĩnh (`-fstack-usage`, tool như StackAnalyzer) + đo động (**stack painting**: fill 0xA5A5A5A5 lúc khởi động, sau đó kiểm tra mức "watermark").
- Stack guard: MPU region ở đáy stack, hoặc canary check định kỳ.

---

## Câu hỏi tự kiểm tra
1. Tại sao không ghi đè trực tiếp một byte trên flash?
2. Fee xử lý mất điện khi đang swap sector như thế nào?
3. Khi nào dùng Redundant NvM block?
4. Tính tuổi thọ: sector 2 KB × 2, bản ghi 16 B, 100k chu kỳ erase, ghi mỗi 10 giây.

## Thực hành
- Viết EEPROM emulation trên PC (mô phỏng flash bằng mảng, xoá = 0xFF, chỉ ghi 1→0), có test "cắt điện" ngẫu nhiên giữa các thao tác.
- Port lên STM32 sử dụng 2 sector flash cuối.
