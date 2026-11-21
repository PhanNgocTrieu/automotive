# 08 – Test & Validation / HIL

## Vai trò công việc
**Chức danh:** Software Test Engineer, Validation Engineer, HIL Engineer, Test Automation Engineer.

**Công việc hằng ngày:**
- Phân tích requirement, viết test specification và test case
- Viết script test tự động (CAPL, Python), chạy test trên HIL/bench
- Unit test và integration test cho phần mềm ECU, đo coverage
- Báo lỗi, phân tích nguyên nhân, viết test report cho release

## Phù hợp nếu bạn
Muốn đi làm sớm, thích tìm lỗi, tự động hoá và nhìn tổng thể cả hệ thống. Đây cũng là bước đệm phổ biến để chuyển sang developer.

## Điều kiện tiên quyết
- [07-development-process](../../automotive-firmware-knowledge/01-automotive-fundamentals/07-development-process.md), [07-debug-and-test](../../automotive-firmware-knowledge/02-firmware/07-debug-and-test.md), [03-diagnostics-uds-obd](../../automotive-firmware-knowledge/01-automotive-fundamentals/03-diagnostics-uds-obd.md)
- Python cơ bản

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | Lý thuyết kiểm thử: ISTQB Foundation, kỹ thuật thiết kế test (equivalence, boundary, decision table, state transition) | 2 |
| 02 | Test theo ASPICE: SWE.4/5/6, test specification, traceability, test report | 1 |
| 03 | Unit test C: Unity/CMock/Ceedling, GoogleTest, mocking register, coverage MC/DC | 3 |
| 04 | Test tự động bằng Python: pytest, python-can, udsoncan, báo cáo | 2 |
| 05 | CANoe & CAPL: simulation node, test module, vTESTstudio | 3 |
| 06 | HIL: kiến trúc, plant model, fault injection (đứt dây, chập, sụt áp), dSPACE/NI | 3 |
| 07 | CI/CD cho embedded: Jenkins/GitLab CI, build + static analysis + test tự động, test trên target | 2 |

**Danh sách bài cần viết:**
- [ ] `01-testing-fundamentals.md`
- [ ] `02-aspice-testing.md`
- [ ] `03-unit-testing-c.md`
- [ ] `04-python-test-automation.md`
- [ ] `05-canoe-capl.md`
- [ ] `06-hil-testing.md`
- [ ] `07-ci-cd-embedded.md`

## Tài liệu
- ISTQB Foundation Level syllabus (miễn phí), ISTQB Automotive Software Tester
- *Test-Driven Development for Embedded C* – James Grenning
- Vector E-Learning: CANoe, CAPL

## Công cụ
| Miễn phí | Thương mại |
|---|---|
| Unity/Ceedling, GoogleTest, gcov/lcov, pytest, python-can, Robot Framework, Jenkins | CANoe + vTESTstudio, VectorCAST, Tessy, ECU-TEST, dSPACE SCALEXIO, NI VeriStand |

## Dự án thực hành
1. **Test suite cho capstone Mini BCM:** test specification có ID gắn với requirement, unit test ≥ 90% branch coverage.
2. **Mini HIL bằng MCU thứ 2:** một board mô phỏng cảm biến/tải (DAC/PWM, relay ngắt dây), Python điều khiển, chạy kịch bản fault injection.
3. **Pipeline CI:** GitHub Actions build firmware, chạy cppcheck và unit test, xuất báo cáo coverage.

## Mốc hoàn thành
- [ ] Viết được test specification từ requirement cho một chức năng
- [ ] Test tự động chạy được không cần người can thiệp, có báo cáo pass/fail
- [ ] Hoàn thành ít nhất 2 trong 3 dự án
- [ ] (Tuỳ chọn) chứng chỉ ISTQB Foundation

## Hướng tiếp theo
Kết hợp với [02 Diagnostics](../02-diagnostics-bootloader-ota/) hoặc [01 BSW](../01-autosar-bsw-mcal/) để chuyển sang developer.
