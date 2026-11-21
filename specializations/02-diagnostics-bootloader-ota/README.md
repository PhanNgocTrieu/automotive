# 02 – Diagnostics, Bootloader & OTA

## Vai trò công việc
**Chức danh:** Diagnostic Engineer, Bootloader/Flash Engineer, OTA Engineer.

**Công việc hằng ngày:**
- Đọc diagnostic specification của OEM (CDD/ODX), cấu hình Dcm/Dem, viết callback DID/routine
- Phát triển và bảo trì bootloader, flash driver, quy trình reprogramming
- Phân tích lỗi flashing ở nhà máy (end-of-line) và xưởng dịch vụ
- Tích hợp OTA: download, verify, cài đặt, rollback

## Phù hợp nếu bạn
Thích giao thức, trình tự chặt chẽ, xử lý mọi tình huống lỗi (mất điện, timeout…).

## Điều kiện tiên quyết
- [03-diagnostics-uds-obd](../../automotive-firmware-knowledge/01-automotive-fundamentals/03-diagnostics-uds-obd.md), [05-bootloader-and-flashing](../../automotive-firmware-knowledge/02-firmware/05-bootloader-and-flashing.md), [04-diagnostic-application](../../automotive-firmware-knowledge/04-application/04-diagnostic-application.md)
- Đã làm Lab A5–A8

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | ISO 14229 đầy đủ: mọi service, sub-function, session/security, timing P2/P2*/S3 | 3 |
| 02 | ISO 15765-2 (ISO-TP) chi tiết, CAN FD, padding, addressing (normal/extended/mixed) | 2 |
| 03 | DoIP (ISO 13400): vehicle discovery, routing activation, gateway | 2 |
| 04 | Dem chuyên sâu: event, DTC, status bit, aging, displacement, OBD (ISO 15031, SAE J1979-2) | 3 |
| 05 | ODX/CDD/PDX, quy trình đặc tả diag của OEM | 1 |
| 06 | Bootloader thiết kế chuẩn công nghiệp: flash driver, memory layout, fingerprint, dependency check | 3 |
| 07 | Secure flashing: chữ ký, Secure Boot, anti-rollback, 0x29 Authentication | 2 |
| 08 | OTA: A/B partition, delta update, UN R156, Uptane, ara::ucm | 4 |

**Danh sách bài cần viết:**
- [ ] `01-uds-complete.md`
- [ ] `02-isotp-deep-dive.md`
- [ ] `03-doip.md`
- [ ] `04-dem-and-obd.md`
- [ ] `05-odx-cdd.md`
- [ ] `06-production-bootloader.md`
- [ ] `07-secure-flashing.md`
- [ ] `08-ota-update.md`

## Tài liệu
- ISO 14229-1, ISO 15765-2, ISO 13400-2, SAE J1979
- AUTOSAR SWS_Dcm, SWS_Dem, SWS_CanTp, SWS_DoIP
- CSS Electronics: UDS, OBD2 tutorials; Vector E-Learning: Diagnostics
- Uptane Standard (uptane.org)

## Công cụ
| Miễn phí | Thương mại |
|---|---|
| python-udsoncan, can-isotp, can-utils, Wireshark (DoIP) | Vector CANoe.DiVa, CANdelaStudio, vFlash |
| iso14229 (C library), SavvyCAN | Softing DTS, ODIS (VW), Bosch ESI |

## Dự án thực hành
1. **UDS server đầy đủ** trên MCU: 15+ service, NRC đúng chuẩn, test tự động bằng pytest + udsoncan.
2. **Bootloader production-grade:** UDS flashing qua CAN FD, flash driver tải vào RAM, kiểm tra CRC + chữ ký ECDSA, chống rollback.
3. **A/B OTA mô phỏng:** "backend" Python → gateway (Linux) → ECU (MCU), cài vào bank B ở nền, chuyển bank, rollback khi lỗi.

## Mốc hoàn thành
- [ ] Viết được trình tự flashing hoàn chỉnh và giải thích từng bước
- [ ] Bootloader chịu được mất điện ở bất kỳ thời điểm nào
- [ ] Hoàn thành ít nhất 2 trong 3 dự án

## Hướng tiếp theo
Kết hợp với [04 Cybersecurity](../04-cybersecurity/) (secure boot, PKI) hoặc [07 HPC](../07-adaptive-autosar-hpc/) (OTA trên Linux).
