# 04 – Automotive Cybersecurity

## Vai trò công việc
**Chức danh:** Automotive Cybersecurity Engineer, Embedded Security Engineer, Penetration Tester (automotive).

**Công việc hằng ngày:**
- Làm TARA, xác định cybersecurity goal và requirement
- Hiện thực Secure Boot, secure flashing, SecOC, quản lý key trong HSM
- Tích hợp crypto stack (AUTOSAR Csm/CryIf/Crypto), PKI, certificate
- Kiểm thử xâm nhập (pentest), fuzzing UDS/CAN, xử lý lỗ hổng

## Phù hợp nếu bạn
Thích mật mã, tư duy như kẻ tấn công, cẩn thận với chi tiết.

## Điều kiện tiên quyết
- [06-cybersecurity](../../automotive-firmware-knowledge/01-automotive-fundamentals/06-cybersecurity.md), [05-bootloader-and-flashing](../../automotive-firmware-knowledge/02-firmware/05-bootloader-and-flashing.md), [03-diagnostics-uds-obd](../../automotive-firmware-knowledge/01-automotive-fundamentals/03-diagnostics-uds-obd.md)

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | Mật mã ứng dụng: AES, CMAC, GCM, SHA, RSA, ECC/ECDSA, RNG, KDF | 3 |
| 02 | ISO/SAE 21434, UN R155/R156, CSMS, TARA (STRIDE, attack tree) | 3 |
| 03 | HSM/SHE/HSE: kiến trúc, key slot, secure storage, lập trình với HSE trên S32K3 | 3 |
| 04 | Secure Boot, chain of trust, secure flashing, anti-rollback | 2 |
| 05 | SecOC, freshness management, key distribution | 2 |
| 06 | PKI & certificate, UDS 0x29 Authentication, TLS cho TCU/HPC | 2 |
| 07 | Bảo mật Ethernet: MACsec, IPsec, firewall; IDS/IDPS trên CAN | 2 |
| 08 | Pentest ECU: fuzzing CAN/UDS, tấn công debug port, side-channel, fault injection cơ bản | 5 |

**Danh sách bài cần viết:**
- [ ] `01-applied-cryptography.md`
- [ ] `02-iso21434-tara.md`
- [ ] `03-hsm-she-hse.md`
- [ ] `04-secure-boot.md`
- [ ] `05-secoc.md`
- [ ] `06-pki-authentication.md`
- [ ] `07-network-security-ids.md`
- [ ] `08-ecu-pentest.md`

## Tài liệu
- ISO/SAE 21434, UN R155/R156, AUTOSAR SWS_SecOC, SWS_Csm, SHE specification
- *The Car Hacker's Handbook* – Craig Smith (miễn phí bản online)
- *Serious Cryptography* – Jean-Philippe Aumasson
- Báo cáo của Miller & Valasek (Jeep hack), Keen Security Lab (Tesla, BMW)

## Công cụ
mbedTLS, wolfSSL, OpenSSL; can-utils, caringcaribou, scapy (CAN/UDS fuzzing); ChipWhisperer (side-channel); Vector vHsm, ESCRYPT CycurHSM (thương mại).

## Dự án thực hành
1. **Secure Boot + secure flashing** trên S32K3 (HSE) hoặc STM32 (mbedTLS): chữ ký ECDSA P-256, version counter.
2. **SecOC trên CAN:** AES-CMAC rút gọn 24 bit + freshness counter, đồng bộ freshness giữa 2 ECU, thử tấn công replay.
3. **Pentest mini BCM:** fuzz UDS với caringcaribou, tìm lỗi trong UDS server của chính mình, viết báo cáo và sửa.

## Mốc hoàn thành
- [ ] Làm được TARA cho một ECU có cổng OBD và kết nối telematics
- [ ] Secure Boot chặn được image bị sửa 1 byte
- [ ] Hoàn thành ít nhất 2 trong 3 dự án

## Hướng tiếp theo
Kết hợp với [02 Diagnostics/OTA](../02-diagnostics-bootloader-ota/) hoặc [07 HPC](../07-adaptive-autosar-hpc/).
