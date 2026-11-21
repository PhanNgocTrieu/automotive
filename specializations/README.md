# Specializations – Các hướng chuyên sâu

Giai đoạn 2 của [ROADMAP](../ROADMAP.md). Chỉ bắt đầu khi đã xong giai đoạn nền tảng ([automotive-firmware-knowledge](../automotive-firmware-knowledge/)).

| # | Hướng | Thời lượng | Trạng thái nội dung |
|---|---|---|---|
| 01 | [AUTOSAR BSW / MCAL](01-autosar-bsw-mcal/) | ~24 tuần | Khung (outline) |
| 02 | [Diagnostics, Bootloader & OTA](02-diagnostics-bootloader-ota/) | ~20 tuần | Khung (outline) |
| 03 | [Functional Safety](03-functional-safety/) | ~20 tuần | Khung (outline) |
| 04 | [Cybersecurity](04-cybersecurity/) | ~22 tuần | Khung (outline) |
| 05 | [Vehicle Networks & Ethernet](05-vehicle-networks-ethernet/) | ~20 tuần | Khung (outline) |
| 06 | [Motor Control & EV](06-motor-control-ev/) | ~24 tuần | Khung (outline) |
| 07 | [Adaptive AUTOSAR & HPC](07-adaptive-autosar-hpc/) | ~24 tuần | Khung (outline) |
| 08 | [Test & Validation / HIL](08-test-validation-hil/) | ~16 tuần | Khung (outline) |

## Cấu trúc chung của mỗi hướng

```
XX-<tên-hướng>/
├── README.md          Vai trò, điều kiện tiên quyết, đề cương module, tài liệu, tool, dự án, mốc hoàn thành
├── 01-<module>.md     ← sẽ viết dần theo đề cương (checkbox trong README)
├── ...
└── labs/              ← code thực hành của hướng
```

Mỗi README có danh sách module dạng checkbox `- [ ]`. Khi viết xong một bài thì đánh dấu `[x]` và cập nhật cột "Trạng thái nội dung" ở bảng trên.
