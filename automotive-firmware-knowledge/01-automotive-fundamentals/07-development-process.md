# 07 – Quy trình phát triển: V-Model, ASPICE

## 1. V-Model

```
 Stakeholder Req ─────────────────────────────────────── Vehicle / Acceptance Test
    System Req ────────────────────────────────────── System Qualification Test
      System Architecture ─────────────────────── System Integration Test
         SW Requirement ───────────────────── SW Qualification Test
            SW Architecture ──────────── SW Integration Test
               SW Detailed Design ── SW Unit Test
                        └── Implementation (Code) ──┘
```
Mỗi bước bên trái có bước kiểm chứng tương ứng bên phải. **Traceability**: Requirement ↔ Design ↔ Code ↔ Test.

## 2. Automotive SPICE (ASPICE)

Mô hình đánh giá năng lực quy trình (dựa ISO/IEC 33000). OEM yêu cầu Tier 1 đạt **Capability Level 2 hoặc 3**.

Các process group chính với software engineer (ASPICE 3.1 / 4.0):
| Process | Nội dung |
|---|---|
| SWE.1 | Software Requirements Analysis |
| SWE.2 | Software Architectural Design |
| SWE.3 | Software Detailed Design & Unit Construction |
| SWE.4 | Software Unit Verification |
| SWE.5 | Software Integration & Integration Test |
| SWE.6 | Software Qualification Test |
| SYS.1–5 | System engineering |
| SUP.1 | Quality Assurance |
| SUP.8 | Configuration Management (Git, baseline) |
| SUP.9 | Problem Resolution Management |
| SUP.10 | Change Request Management |
| MAN.3 | Project Management |

Capability levels: 0 Incomplete, 1 Performed, 2 Managed, 3 Established, 4 Predictable, 5 Innovating.

## 3. Viết requirement tốt

Một requirement tốt: **rõ ràng, kiểm chứng được, nguyên tử, có ID, có lý do**.

❌ "Đèn phải bật nhanh."
✅ `SWR-HL-012`: Khi tín hiệu `LightSwitch` = ON trong ≥ 50 ms, phần mềm phải đặt `HeadlightCmd` = ON trong vòng 100 ms. *(ASIL B, verify: SWE.6 test HL_TC_012)*

Tool quản lý requirement: **IBM DOORS (Next)**, **Polarion**, **Jama**, **Codebeamer**.

## 4. Thiết kế & tài liệu
- Architecture: UML/SysML (Enterprise Architect, Rhapsody), ARXML cho AUTOSAR.
- Detailed design: sequence diagram, state machine, flowchart, interface spec.
- Doxygen cho code-level.

## 5. Kiểm thử
| Mức | Công cụ ví dụ |
|---|---|
| Unit test | **Unity/CMock, Ceedling**, GoogleTest, **VectorCAST**, **Tessy**, Cantata |
| Static analysis | PC-lint, QAC (Helix), Polyspace, cppcheck, clang-tidy |
| Integration test | Trên target / SIL |
| HIL (Hardware-in-the-Loop) | dSPACE, NI VeriStand, Vector VT System |
| Coverage | gcov/lcov, Testwell CTC++, VectorCAST |
| Test automation | CANoe CAPL, Python, ECU-TEST (tracetronic) |

## 6. Configuration & change management
- Git (branching: feature → develop → release), **baseline/tag** cho mỗi release.
- Code review bắt buộc (Gerrit, GitHub/GitLab MR).
- CI: build + static analysis + unit test tự động (Jenkins, GitLab CI).
- Mỗi bug/feature có ticket (Jira) gắn với commit.

## 7. Release
- Release note, known issues, version (major.minor.patch), memory/CPU load report, test report, MISRA deviation report.

---

## Câu hỏi tự kiểm tra
1. Tại sao V-model quan trọng với safety?
2. SWE.4 và SWE.6 khác nhau thế nào?
3. Viết lại requirement "Hệ thống phải phát hiện quá áp" theo chuẩn.
4. Traceability giúp gì khi có change request?
