# 06 – Automotive Cybersecurity

## 1. Chuẩn & quy định
- **ISO/SAE 21434:** quy trình kỹ thuật an ninh mạng cho xe (tương tự ISO 26262 cho security).
- **UN R155:** bắt buộc OEM có CSMS (Cyber Security Management System) để được phê duyệt kiểu xe (EU, nhiều nước từ 7/2024).
- **UN R156:** bắt buộc SUMS (Software Update Management System) – liên quan OTA.

**TARA (Threat Analysis and Risk Assessment):** xác định asset → threat scenario (STRIDE) → attack path → impact & feasibility → risk → cybersecurity goal (tương tự HARA).

## 2. Bề mặt tấn công
- Cổng OBD-II (truy cập trực tiếp CAN/diag).
- Telematics (4G/5G), Wi-Fi, Bluetooth, Infotainment.
- Key fob / PKES (relay attack).
- Cảm biến (spoofing camera/radar/GPS).
- Cổng debug JTAG/SWD trên ECU.
- Cập nhật phần mềm (OTA, flashing).

Ví dụ nổi tiếng: Jeep Cherokee hack 2015 (Miller & Valasek) – từ Uconnect qua mạng di động tới CAN bus.

## 3. Cơ chế bảo mật mà firmware cần biết

| Cơ chế | Mô tả |
|---|---|
| **HSM / SHE / HSE** | Hardware Security Module trong MCU (Infineon HSM, NXP HSE/CSEc, Renesas ICU) lưu key, chạy AES/RSA/ECC |
| **Secure Boot** | Kiểm tra chữ ký/MAC của firmware trước khi chạy (chain of trust: ROM → bootloader → application) |
| **Secure Flashing** | Chỉ chấp nhận image có chữ ký hợp lệ; chống downgrade (version counter) |
| **SecOC** (AUTOSAR) | Thêm **MAC (CMAC-AES) + Freshness Value** vào CAN message → chống giả mạo & replay |
| **UDS 0x27 / 0x29** | Security access / authentication bằng certificate |
| **Debug protection** | Khoá JTAG/SWD bằng password hoặc vĩnh viễn ở production |
| **Secure storage** | Key, credential lưu trong HSM, không trong flash thường |
| **IDS / IDPS** | Phát hiện xâm nhập trên CAN/Ethernet (tần suất message bất thường) |
| **Firewall gateway** | Lọc message giữa các domain |
| **TLS, MACsec, IPsec** | Bảo mật Ethernet / kết nối ngoài |

## 4. Nguyên tắc code an toàn
- **CERT C** / MISRA C – tránh buffer overflow, integer overflow.
- **Kiểm tra độ dài** mọi dữ liệu nhận từ bus (UDS request length, ISO-TP length).
- Không để lại backdoor, debug command trong release build.
- So sánh MAC/key bằng **constant-time compare**.
- Seed phải ngẫu nhiên thật (TRNG), không dùng seed cố định/đoán được.
- Giới hạn số lần thử key sai + delay (NRC 0x36/0x37).

```c
/* constant-time compare – tránh timing attack */
static bool ct_equal(const uint8_t *a, const uint8_t *b, size_t n)
{
    uint8_t diff = 0u;
    for (size_t i = 0u; i < n; i++) {
        diff |= (uint8_t)(a[i] ^ b[i]);
    }
    return (diff == 0u);
}
```

---

## Câu hỏi tự kiểm tra
1. Secure Boot hoạt động như thế nào? Chain of trust là gì?
2. SecOC chống được loại tấn công nào? Freshness value để làm gì?
3. Tại sao cần khoá cổng debug ở production?
4. Tìm lỗi bảo mật trong một handler UDS không kiểm tra độ dài request.

## Thực hành
- Dùng thư viện mbedTLS / tinycrypt tính AES-CMAC cho một CAN payload 4 byte + freshness counter 4 byte, cắt MAC còn 24-bit như SecOC.
- Viết bootloader mô phỏng (trên PC) kiểm tra SHA-256 + chữ ký ECDSA của image trước khi "jump".
