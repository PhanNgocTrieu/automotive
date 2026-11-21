# 03 – Functional Safety (ISO 26262)

## Vai trò công việc
**Chức danh:** Functional Safety Engineer, Safety SW Engineer, Functional Safety Manager (khi có kinh nghiệm).

**Công việc hằng ngày:**
- Viết safety requirement (FSR, TSR, SW safety requirement), duy trì traceability
- Làm phân tích FMEA, FTA, DFA ở mức hệ thống/phần mềm
- Thiết kế và hiện thực safety mechanism (watchdog, E2E, memory test, plausibility…)
- Chuẩn bị bằng chứng cho safety case, làm việc với assessor (TÜV…)

## Phù hợp nếu bạn
Tư duy hệ thống, kỹ tính, thích phân tích "nếu cái này hỏng thì chuyện gì xảy ra".

## Điều kiện tiên quyết
- [05-functional-safety](../../automotive-firmware-knowledge/01-automotive-fundamentals/05-functional-safety.md), [07-development-process](../../automotive-firmware-knowledge/01-automotive-fundamentals/07-development-process.md), [08-coding-standards](../../automotive-firmware-knowledge/01-automotive-fundamentals/08-coding-standards.md)
- Đã làm Lab A10

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | Cấu trúc ISO 26262 (12 phần), safety lifecycle, safety plan, safety case | 2 |
| 02 | Item definition, HARA, ASIL, safety goal | 2 |
| 03 | Functional & technical safety concept, ASIL decomposition (Part 9) | 3 |
| 04 | Phân tích: FMEA, FTA, DFA, FMEDA; metric phần cứng SPFM, LFM, PMHF | 3 |
| 05 | Software safety (Part 6): kiến trúc, FFI, MPU, timing protection, coverage MC/DC | 3 |
| 06 | Safety mechanism trên MCU: lockstep, ECC, BIST, safety manual của AURIX/S32K3 | 3 |
| 07 | Tool qualification, confirmation measures, safety assessment | 2 |
| 08 | SOTIF (ISO 21448) và an toàn cho ADAS | 2 |

**Danh sách bài cần viết:**
- [ ] `01-iso26262-lifecycle.md`
- [ ] `02-hara-asil.md`
- [ ] `03-safety-concept-decomposition.md`
- [ ] `04-fmea-fta-fmeda.md`
- [ ] `05-software-safety-ffi.md`
- [ ] `06-mcu-safety-mechanisms.md`
- [ ] `07-tool-qualification-assessment.md`
- [ ] `08-sotif.md`

## Tài liệu
- ISO 26262:2018 (Part 1–12), ISO 21448
- *Functional Safety for Road Vehicles* – Hans-Leo Ross
- Safety manual của Infineon AURIX TC3xx, NXP S32K3
- Whitepaper của exida, TÜV SÜD, Kugler Maag

## Công cụ
Medini analyze (ANSYS), APIS IQ-FMEA, Isograph FaultTree+, Polyspace Code Prover, VectorCAST/Tessy (coverage), DOORS/Polarion (traceability).

## Dự án thực hành
1. **Safety concept cho bàn đạp ga điện tử (E-Gas):** item definition → HARA → safety goal → FSC → TSC, kèm sơ đồ 3 lớp (E-Gas monitoring concept).
2. **Safety mechanism library trên S32K3/AURIX:** RAM/Flash test lúc khởi động, program flow monitoring, E2E profile, safe state handler, có fault injection test.
3. **FMEA + FTA** cho capstone Mini BCM (chức năng đèn pha, ASIL B).

## Mốc hoàn thành
- [ ] Làm được HARA và xác định ASIL cho một chức năng thực tế
- [ ] Giải thích được FFI và cách chứng minh trên một MCU cụ thể
- [ ] Hoàn thành dự án 1 và ít nhất một trong hai dự án còn lại
- [ ] (Tuỳ chọn) chứng chỉ Functional Safety Engineer của TÜV

## Hướng tiếp theo
Kết hợp với [01 BSW](../01-autosar-bsw-mcal/) hoặc [06 Motor Control/EV](../06-motor-control-ev/).
