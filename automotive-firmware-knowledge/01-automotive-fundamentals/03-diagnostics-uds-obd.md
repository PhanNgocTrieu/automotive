# 03 – Chẩn đoán: UDS, OBD-II, DTC

## 1. Tại sao cần chẩn đoán?
- Phát hiện & lưu lỗi (DTC) để sửa chữa.
- Đọc dữ liệu runtime (sensor, version phần mềm).
- Điều khiển actuator để kiểm tra (I/O control).
- **Nạp phần mềm (flashing / reprogramming)** – xưởng, end-of-line, OTA.
- Yêu cầu pháp lý về khí thải (OBD).

## 2. Kiến trúc giao tiếp
```
Tester (PC + VCI / CANoe / ODIS)  ◄── CAN (ISO-TP) / DoIP ──►  ECU (Dcm + Dem)
         Request: [SID][Sub-func][Data...]
         Positive response: [SID + 0x40][...]
         Negative response: [0x7F][SID][NRC]
```
- Địa chỉ: **physical** (1 ECU, ví dụ 0x7E0 → resp 0x7E8) và **functional** (broadcast, 0x7DF).

## 3. UDS – ISO 14229 (Unified Diagnostic Services)

| SID | Service | Mô tả |
|---|---|---|
| 0x10 | DiagnosticSessionControl | 01 Default, 02 Programming, 03 Extended |
| 0x11 | ECUReset | 01 Hard, 03 Soft |
| 0x14 | ClearDiagnosticInformation | Xoá DTC |
| 0x19 | ReadDTCInformation | Đọc DTC (sub 02: by status mask, 04: snapshot, 06: extended data) |
| 0x22 | ReadDataByIdentifier | Đọc DID (ví dụ F190 VIN, F189 SW version) |
| 0x23 | ReadMemoryByAddress | |
| 0x27 | SecurityAccess | Seed/Key: request seed (odd) → send key (even) |
| 0x28 | CommunicationControl | Tắt/bật Tx/Rx message thường (khi flashing) |
| 0x29 | Authentication | Thay 0x27 bằng chứng chỉ (UDS 2020) |
| 0x2E | WriteDataByIdentifier | Ghi DID (VIN, cấu hình) |
| 0x2F | InputOutputControlByIdentifier | Điều khiển actuator |
| 0x31 | RoutineControl | Chạy routine: erase memory, check dependency… |
| 0x34 | RequestDownload | Bắt đầu tải phần mềm |
| 0x36 | TransferData | Gửi từng block |
| 0x37 | RequestTransferExit | Kết thúc |
| 0x3E | TesterPresent | Giữ session không timeout |
| 0x85 | ControlDTCSetting | Tắt ghi DTC khi flashing |

### NRC (Negative Response Code) phổ biến
| NRC | Ý nghĩa |
|---|---|
| 0x11 | serviceNotSupported |
| 0x12 | subFunctionNotSupported |
| 0x13 | incorrectMessageLengthOrInvalidFormat |
| 0x22 | conditionsNotCorrect |
| 0x24 | requestSequenceError |
| 0x31 | requestOutOfRange |
| 0x33 | securityAccessDenied |
| 0x35 | invalidKey |
| 0x36 | exceededNumberOfAttempts |
| 0x37 | requiredTimeDelayNotExpired |
| 0x72 | generalProgrammingFailure |
| 0x78 | **responsePending** – ECU cần thêm thời gian (P2* timer) |
| 0x7E/0x7F | sub-function/service not supported in active session |

### Timing
- **P2server:** thời gian tối đa ECU phải trả lời (thường 50 ms).
- **P2\*server:** sau khi gửi NRC 0x78 (thường 5000 ms).
- **S3server:** session non-default tự quay về default nếu không có request (5 s) → dùng TesterPresent.

### Ví dụ trao đổi
```
Tester → 22 F1 90                  (đọc VIN)
ECU    ← 62 F1 90 57 56 57 ...     (positive, 0x22+0x40=0x62)

Tester → 10 03                     (extended session)
ECU    ← 50 03 00 32 01 F4         (P2=50ms, P2*=500×10ms)

Tester → 27 01                     (request seed)
ECU    ← 67 01 A1 B2 C3 D4
Tester → 27 02 <key>
ECU    ← 67 02                     hoặc 7F 27 35 (invalid key)
```

### Trình tự flashing điển hình
```
10 03 → 85 02 (DTC off) → 28 03 (Com off) → 10 02 (programming, ECU nhảy vào bootloader)
→ 27 xx (security) → 31 01 FF00 (erase) → 34 (request download) → 36 ×N → 37
→ 31 01 FF01 (check memory / signature) → 11 01 (reset)
```

## 4. DTC (Diagnostic Trouble Code)

- Định dạng 3 byte (UDS) + 1 byte **status**. OBD: ví dụ `P0301` (Misfire cylinder 1). Chữ cái: **P** Powertrain, **C** Chassis, **B** Body, **U** Network.
- **DTC Status byte (ISO 14229):**

| Bit | Tên |
|---|---|
| 0 | testFailed |
| 1 | testFailedThisOperationCycle |
| 2 | pendingDTC |
| 3 | confirmedDTC |
| 4 | testNotCompletedSinceLastClear |
| 5 | testFailedSinceLastClear |
| 6 | testNotCompletedThisOperationCycle |
| 7 | warningIndicatorRequested (đèn MIL) |

- **Snapshot (freeze frame):** dữ liệu môi trường lúc lỗi xảy ra (áp, nhiệt, tốc độ, odometer).
- **Extended data:** counters (occurrence, aging).
- **Debouncing:** counter-based hoặc time-based trước khi báo lỗi (xem `04-application/04-diagnostic-application.md`).
- **Aging / healing:** DTC tự xoá sau N chu kỳ không lỗi.
- **Operation cycle:** thường là chu kỳ ignition ON→OFF.

## 5. OBD-II (ISO 15031 / SAE J1979)
- Bắt buộc với xe động cơ đốt (khí thải). Cổng **OBD-II 16 chân** (CAN chân 6/14, K-Line chân 7).
- Mode/Service: 01 Live data (PID 0x0C RPM, 0x0D speed), 03 đọc DTC, 04 xoá DTC, 09 VIN...
- Ví dụ: `7DF: 02 01 0C` → `7E8: 04 41 0C 1A F8` → RPM = (0x1AF8)/4 = 1726 rpm.

## 6. Các chuẩn khác
- **ODX (ASAM MCD-2D):** mô tả dữ liệu chẩn đoán cho tester.
- **CDD (Vector CANdelaStudio):** file đặc tả diag, generate cấu hình Dcm/Dem.
- **DoIP (ISO 13400):** UDS qua Ethernet.
- **XCP / CCP (ASAM):** đo lường & calibration (không phải diag nhưng hay đi cùng) – dùng với INCA/CANape.

---

## Câu hỏi tự kiểm tra
1. Response của `22 F1 89` là gì nếu thành công? Nếu DID không tồn tại?
2. Khi nào ECU gửi NRC 0x78? Firmware cần làm gì?
3. Vì sao phải tắt DTC setting và communication khi flashing?
4. DTC status = 0x2F nghĩa là gì?
5. Seed/Key bảo vệ điều gì? Điểm yếu?

## Thực hành
- Trên vcan: dùng `isotpsend`/`isotprecv` (can-utils) hoặc `python-udsoncan` + `can-isotp` để viết một UDS server mô phỏng hỗ trợ 0x10, 0x22, 0x27, 0x3E. (Xem `05-labs-projects`.)
