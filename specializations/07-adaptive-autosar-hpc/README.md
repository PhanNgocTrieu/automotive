# 07 – Adaptive AUTOSAR & High-Performance Computing (HPC)

## Vai trò công việc
**Chức danh:** Adaptive AUTOSAR Developer, Embedded Linux Engineer, HPC Platform Engineer, SDV Software Engineer.

**Công việc hằng ngày:**
- Phát triển ứng dụng và service C++ trên Linux/QNX cho central compute, ADAS, gateway
- Cấu hình Adaptive AUTOSAR (ara::com, execution manager, manifest)
- Bring-up BSP, Yocto, device tree, driver Linux
- Làm việc với hypervisor, container, OTA trên SoC

## Phù hợp nếu bạn
Thích C++ hiện đại, Linux, phần mềm quy mô lớn và kiến trúc hướng dịch vụ.

## Điều kiện tiên quyết
- [01-vehicle-ee-architecture](../../automotive-firmware-knowledge/01-automotive-fundamentals/01-vehicle-ee-architecture.md), [04-autosar](../../automotive-firmware-knowledge/01-automotive-fundamentals/04-autosar.md) (phần Adaptive), [02-in-vehicle-networks](../../automotive-firmware-knowledge/01-automotive-fundamentals/02-in-vehicle-networks.md) (phần Ethernet)
- Thành thạo Linux command line, Git

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | Modern C++ (14/17): RAII, smart pointer, move, template, STL; AUTOSAR C++14 / MISRA C++:2023 | 4 |
| 02 | Linux systems programming: process, thread, IPC, socket, real-time (PREEMPT_RT) | 3 |
| 03 | Embedded Linux: boot flow, U-Boot, device tree, kernel driver cơ bản, Yocto | 4 |
| 04 | SOME/IP & vsomeip, DDS; service-oriented architecture | 2 |
| 05 | Adaptive AUTOSAR: ara::com, ara::exec, ara::per, ara::diag, ara::log, manifest | 4 |
| 06 | QNX, hypervisor (Xen, QNX Hypervisor), mixed-criticality với safety MCU | 2 |
| 07 | SDV: container, OTA trên SoC, middleware COVESA/Eclipse SDV | 3 |
| 08 | Hiệu năng & debug: perf, ftrace, gdb, valgrind, sanitizer | 2 |

**Danh sách bài cần viết:**
- [ ] `01-modern-cpp-automotive.md`
- [ ] `02-linux-systems-programming.md`
- [ ] `03-embedded-linux-yocto.md`
- [ ] `04-someip-dds.md`
- [ ] `05-adaptive-autosar.md`
- [ ] `06-qnx-hypervisor.md`
- [ ] `07-software-defined-vehicle.md`
- [ ] `08-performance-debugging.md`

## Tài liệu
- AUTOSAR Adaptive Platform specifications (EXP_PlatformDesign, SWS_CommunicationManagement)
- *Effective Modern C++* – Scott Meyers; *The Linux Programming Interface* – Michael Kerrisk
- *Mastering Embedded Linux Programming* – Chris Simmonds
- COVESA (VSS, vsomeip), Eclipse SDV, Bootlin training materials (miễn phí)

## Công cụ
GCC/Clang, CMake, Yocto/Buildroot, QEMU, Docker, vsomeip, CycloneDDS, Raspberry Pi / NXP i.MX8 / S32G-VNP-RDB; Vector MICROSAR Adaptive, Elektrobit corbos (thương mại).

## Dự án thực hành
1. **Service SOME/IP trên Linux:** service "VehicleSpeed" (provider) và "Cluster" (consumer) bằng vsomeip, có service discovery.
2. **Image Yocto tuỳ chỉnh** cho Raspberry Pi/i.MX: thêm ứng dụng, systemd service, SocketCAN.
3. **Mixed-criticality:** Linux (HPC) giao tiếp với MCU an toàn qua SPI/Ethernet, có heartbeat và safe state khi Linux treo.

## Mốc hoàn thành
- [ ] Viết C++ không rò rỉ tài nguyên, qua được sanitizer và clang-tidy
- [ ] Build được image Linux tuỳ chỉnh và driver kernel đơn giản
- [ ] Hoàn thành ít nhất 2 trong 3 dự án

## Hướng tiếp theo
Kết hợp với [04 Cybersecurity](../04-cybersecurity/) hoặc [05 Networks](../05-vehicle-networks-ethernet/).
