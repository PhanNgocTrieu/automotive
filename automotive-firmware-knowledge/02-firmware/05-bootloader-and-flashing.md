# 05 – Bootloader, Flashing & OTA

## 1. Bootloader là gì?
Chương trình nhỏ chạy đầu tiên sau reset, quyết định:
- Chạy application (nếu hợp lệ), hoặc
- Ở lại để nạp phần mềm mới qua CAN/LIN/Ethernet (UDS).

ECU automotive **luôn** có bootloader riêng (không dùng ROM bootloader của chip ở production) để hỗ trợ reprogram tại xưởng/OTA.

## 2. Bố cục bộ nhớ điển hình
```
0x0000_0000 ┌──────────────────────────┐
            │ Bootloader (32–128 KB)   │  ← không bao giờ bị xoá qua UDS
            ├──────────────────────────┤
            │ App header               │  version, size, CRC, signature, valid flag
            ├──────────────────────────┤
            │ Application              │
            ├──────────────────────────┤
            │ Calibration data         │
            ├──────────────────────────┤
            │ NvM / EEPROM emulation   │  (D-Flash)
            └──────────────────────────┘
RAM:  vùng shared/noinit chứa "programming request flag" + key magic
```

## 3. Luồng khởi động
```
Reset
 └─► Bootloader init tối thiểu (clock, WDG)
      ├─ Có programming request flag (từ app nhận 10 02)?  ── Yes ─► Ở lại bootloader, trả lời 50 02
      ├─ App header hợp lệ (magic, CRC/signature OK)?      ── No  ─► Ở lại bootloader
      └─ Yes ─► Deinit peripheral, set VTOR, set MSP, jump app
```

Jump sang application trên Cortex-M:
```c
typedef void (*AppEntry_t)(void);

void Boot_JumpToApp(uint32_t appBase)
{
    uint32_t sp    = *(volatile uint32_t *)(appBase + 0u);
    uint32_t reset = *(volatile uint32_t *)(appBase + 4u);

    __disable_irq();
    /* deinit SysTick, peripheral, clear pending IRQ */
    SysTick->CTRL = 0u;
    for (uint32_t i = 0u; i < 8u; i++) {
        NVIC->ICER[i] = 0xFFFFFFFFu;
        NVIC->ICPR[i] = 0xFFFFFFFFu;
    }
    SCB->VTOR = appBase;
    __DSB(); __ISB();
    __set_MSP(sp);
    __enable_irq();
    ((AppEntry_t)reset)();
}
```

## 4. Flash programming
- Flash phải **xoá theo sector** (về 0xFF) trước khi ghi; ghi theo phrase/page (8, 16, 32 byte…).
- Thường không thể chạy code từ cùng một flash bank đang bị xoá/ghi → chạy flash driver từ **RAM**, hoặc dùng bank khác (read-while-write).
- Flash driver đôi khi được **download vào RAM** bởi tester lúc flashing (không lưu sẵn trong ECU → chống ghi nhầm flash).
- Phải **phục vụ watchdog** trong lúc xoá dài.
- Kiểm tra điện áp đủ trước khi xoá/ghi.

## 5. Quy trình UDS flashing
Xem `01-automotive-fundamentals/03-diagnostics-uds-obd.md`:
```
Pre-programming (app):  10 03 → 31 01 0203 (check preconditions) → 85 02 → 28 03 → 10 02 ─► reset vào BL
Programming (BL):       27 11/12 → 2E F15A (fingerprint) → 31 01 FF00 (erase)
                        → 34 → 36… → 37 → 31 01 0202 (check CRC/signature)
                        → 31 01 FF01 (check dependencies) → 11 01
Post (app):             14 FF FF FF (clear DTC)
```
File phần mềm: **.hex / .s19 / .vbf (Volvo) / .odx-f / .pdx**.

## 6. An toàn & bảo mật khi flash
- **Valid flag ghi cuối cùng**: chỉ đánh dấu app hợp lệ sau khi ghi + verify xong → mất điện giữa chừng vẫn còn bootloader.
- **CRC32/SHA-256 + chữ ký** (RSA/ECDSA) → Secure flashing.
- **Anti-rollback:** version counter.
- Bootloader update: rất rủi ro → thường dùng "bootloader updater" app tạm, hoặc dual-bootloader.

## 7. A/B partition & OTA
- **A/B (dual bank):** chạy bank A, ghi bản mới vào bank B ở nền trong khi xe vẫn chạy, reboot sang B; lỗi → rollback A. MCU như S32K3, AURIX TC3xx, RH850 U2A hỗ trợ.
- **OTA flow:** Backend → TCU (download, verify) → Gateway/OTA master → ECU đích (UDS hoặc ara::ucm).
- Chuẩn **UN R156** yêu cầu quy trình quản lý cập nhật.
- Khung tham khảo: **Uptane** (bảo mật OTA).

---

## Câu hỏi tự kiểm tra
1. Vì sao "valid flag" phải ghi sau cùng?
2. Tại sao không xoá flash trong khi chạy code từ cùng bank?
3. Liệt kê các bước trước khi jump từ bootloader sang app.
4. Làm sao app báo cho bootloader rằng cần vào programming mode?
5. A/B partition giải quyết vấn đề gì?

## Thực hành
- Trên STM32: chia flash 32KB bootloader + app. Bootloader nhận file .bin qua UART (protocol đơn giản: header + chunk + CRC), ghi flash, verify, jump.
- Nâng cấp: thay UART bằng CAN + ISO-TP + UDS 0x34/0x36/0x37.
