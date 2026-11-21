# 05 – Functional Safety (ISO 26262)

**Functional Safety:** Không có rủi ro không chấp nhận được do hành vi sai của hệ thống E/E gây ra bởi lỗi (fault).

ISO 26262 (bản 2018) gồm 12 phần. Firmware engineer chủ yếu làm việc với **Part 6 (Software)**, liên quan Part 4 (System), Part 5 (Hardware), Part 8 (Supporting processes), Part 9 (ASIL decomposition).

## 1. HARA & ASIL

**HARA (Hazard Analysis and Risk Assessment)** → xác định **ASIL** cho từng Safety Goal dựa trên:

| Tham số | Mức |
|---|---|
| **S** – Severity (mức độ thương tích) | S0–S3 |
| **E** – Exposure (tần suất gặp tình huống) | E0–E4 |
| **C** – Controllability (khả năng người lái kiểm soát) | C0–C3 |

→ **QM** (quality managed – không yêu cầu safety đặc biệt), **ASIL A, B, C, D** (D nghiêm ngặt nhất).

Ví dụ:
| Chức năng | ASIL điển hình |
|---|---|
| Airbag kích nổ sai/không nổ | D |
| Phanh, lái điện (EPS) | D |
| Tăng tốc ngoài ý muốn | C–D |
| Đèn pha (tắt đột ngột ban đêm) | B |
| Đèn phanh | A–B |
| Infotainment | QM |

## 2. Chuỗi yêu cầu an toàn
```
Safety Goal (SG) → Functional Safety Requirement (FSR) → Technical Safety Requirement (TSR)
→ HW Safety Req / SW Safety Requirement (SWSR) → Implementation → Verification
```
Phải có **traceability** hai chiều.

## 3. Fault, Error, Failure
- **Fault:** nguyên nhân (bit flip, bug, chân bị hở).
- **Error:** trạng thái sai bên trong.
- **Failure:** hệ thống không thực hiện đúng chức năng.
- **Systematic fault** (bug, thiết kế sai) → chống bằng **process** (V-model, review, test).
- **Random hardware fault** (bit flip, linh kiện hỏng) → chống bằng **safety mechanism**, đánh giá bằng metric SPFM, LFM, PMHF.

## 4. Safety mechanism thường gặp trong firmware

| Cơ chế | Phát hiện |
|---|---|
| **Watchdog** (internal + external window WDG) | CPU treo, chương trình chạy sai |
| **Program flow monitoring** (WdgM logical supervision, checkpoint) | Code chạy sai thứ tự |
| **Lockstep core** (TriCore, Cortex-R5 lockstep) | Lỗi CPU (HW) |
| **ECC RAM/Flash** | Bit flip bộ nhớ |
| **Memory test** (March test RAM khi start-up, Flash CRC) | Lỗi bộ nhớ |
| **MPU (Memory Protection Unit)** | SW ASIL thấp ghi đè vùng nhớ ASIL cao (freedom from interference) |
| **E2E protection** (CRC + counter) | Lỗi truyền thông |
| **Redundant sensor + plausibility** | Cảm biến lỗi (ví dụ 2 kênh APP) |
| **ADC self-test, reference check** | ADC lỗi |
| **Clock monitor** | Mất clock / clock sai |
| **Voltage monitor** | Nguồn ngoài dải |
| **Readback output** | Output không đúng lệnh |
| **Data redundancy** (lưu biến + bản bù ~x) | Biến bị hỏng |

**Safe state:** trạng thái an toàn khi phát hiện lỗi (ví dụ tắt motor, giữ đèn bật, chuyển limp-home). **FTTI (Fault Tolerant Time Interval):** thời gian tối đa từ khi lỗi xảy ra đến khi phải vào safe state.

## 5. Freedom From Interference (FFI)
Khi SW ASIL khác nhau cùng chạy trên 1 MCU, phải chứng minh phần ASIL thấp không làm hỏng phần ASIL cao:
- **Spatial:** MPU, phân vùng memory.
- **Temporal:** timing protection của OS, deadline monitoring.
- **Communication:** E2E.

## 6. Yêu cầu ISO 26262-6 với software

| Hạng mục | Ví dụ khuyến nghị (tăng dần theo ASIL) |
|---|---|
| Coding guidelines | **MISRA C** (bắt buộc thực tế) |
| Thiết kế | Modular, giới hạn độ phức tạp, không đệ quy, không dynamic memory, hạn chế pointer, single entry/exit |
| Unit test | Requirement-based, interface test, fault injection, boundary value |
| **Structural coverage** | Statement (A), Branch (B,C), **MC/DC** (D) |
| Static analysis | Polyspace, QAC, Coverity, Astrée… |
| Tool qualification | Đánh giá TCL cho compiler, test tool |

## 7. Các tiêu chuẩn liên quan
- **ISO 21448 (SOTIF):** an toàn của chức năng dự định (ADAS – khi hệ thống không lỗi nhưng hiểu sai môi trường).
- **IEC 61508:** chuẩn gốc functional safety công nghiệp.
- **FMEA, FTA, DFA:** phương pháp phân tích.

---

## Câu hỏi tự kiểm tra
1. ASIL được xác định từ những yếu tố nào?
2. Phân biệt systematic fault và random hardware fault; biện pháp với mỗi loại.
3. FFI là gì? Ba khía cạnh?
4. MC/DC coverage khác branch coverage như thế nào? Ví dụ với `if (a && b)`.
5. Thiết kế safety mechanism cho cảm biến bàn đạp ga 2 kênh.

## Thực hành
- Viết module `SafeVar` lưu một biến kèm bản bù (`~value`), kiểm tra mỗi lần đọc.
- Viết RAM March C- test chạy trên một vùng RAM thử nghiệm.
- Viết program flow monitor đơn giản: checkpoint ID cộng dồn vào signature, kiểm tra cuối chu kỳ.
