# Tài nguyên học tập

## Sách

### Điện tử
- *The Art of Electronics* – Horowitz & Hill (kinh điển, tra cứu)
- *Practical Electronics for Inventors* – Scherz & Monk (dễ đọc cho người mới)
- *Automotive Electrics & Automotive Electronics* – Bosch (Bosch Professional Automotive Information)

### Embedded / Firmware
- *Making Embedded Systems* – Elecia White (rất nên đọc đầu tiên)
- *The Definitive Guide to ARM Cortex-M3 and Cortex-M4 Processors* – Joseph Yiu
- *Embedded C Coding Standard* – Michael Barr (Barr Group)
- *Test-Driven Development for Embedded C* – James Grenning
- *Mastering the FreeRTOS Real Time Kernel* – Richard Barry (miễn phí tại freertos.org)
- *Better Embedded System Software* – Philip Koopman
- *Real-Time Systems* / *Hard Real-Time Computing Systems* – Giorgio Buttazzo

### Automotive
- *Automotive Embedded Systems Handbook* – Navet & Simonot-Lion
- *Controller Area Network* – Konrad Etschberger; *A Comprehensible Guide to CAN* (CSS Electronics, miễn phí)
- *Automotive SPICE in Practice* – Hoermann et al.
- *Functional Safety for Road Vehicles* – Hans-Leo Ross
- *Automotive Software Architectures* – Miroslaw Staron
- *Understanding Automotive Electronics* – William Ribbens

## Tiêu chuẩn (bản chính thức có phí; xem tóm tắt/whitepaper trước)
- ISO 11898 (CAN), ISO 17987 (LIN), ISO 14229 (UDS), ISO 15765 (ISO-TP), ISO 13400 (DoIP)
- ISO 26262 (Functional Safety), ISO 21448 (SOTIF), ISO/SAE 21434 (Cybersecurity)
- ISO 16750 / ISO 7637 (môi trường điện), CISPR 25 (EMC)
- MISRA C:2012/2023, Automotive SPICE PAM (miễn phí tại vda-qmc.de)
- **AUTOSAR specifications – miễn phí tại autosar.org** (SWS_Can, SWS_Com, SWS_Dcm…)

## Website / Blog / Khoá học
- **CSS Electronics** – hướng dẫn CAN, CAN FD, UDS, OBD, DBC rất dễ hiểu
- **Vector E-Learning** (miễn phí): CAN, LIN, AUTOSAR, Diagnostics, Ethernet
- **Interrupt (Memfault blog)** – firmware best practices
- **Embedded Artistry**
- Quantum Leaps (Miro Samek) – YouTube "Modern Embedded Systems Programming" (bare-metal, state machine, RTOS)
- Fastbit Embedded Brain Academy (Udemy) – bare-metal STM32, drivers
- Coursera: "Introduction to Embedded Systems Software" (CU Boulder)
- NXP / Infineon / Renesas training portals

## Công cụ miễn phí
| Mục đích | Tool |
|---|---|
| Mô phỏng mạch | LTspice, Falstad, KiCad (schematic/PCB), Logisim Evolution |
| Toolchain | arm-none-eabi-gcc, CMake, Make |
| IDE | VS Code + Cortex-Debug, STM32CubeIDE, **NXP S32 Design Studio** (có MCAL RTD miễn phí) |
| Debug | OpenOCD, GDB, Segger J-Link tools, PulseView (sigrok) |
| Mô phỏng MCU | QEMU, Renode |
| CAN | SocketCAN + can-utils, python-can, cantools, SavvyCAN, BusMaster, Wireshark |
| Diag | python-udsoncan, can-isotp |
| Test | Unity, CMock, Ceedling, GoogleTest, gcov/lcov |
| Static analysis | cppcheck (MISRA addon), clang-tidy |
| RTOS | FreeRTOS, Zephyr |
| Version control | Git |

## Open-source tham khảo
- FreeRTOS kernel – đọc mã nguồn scheduler (`tasks.c`, port Cortex-M)
- Zephyr drivers – xem cách tổ chức driver chuyên nghiệp
- **openSAE J1939, uds-c, iso14229 (driftregion/iso14229)** – UDS server/client C
- **CANopenNode**, **libopencm3**
- Linux kernel `drivers/net/can/` – SocketCAN drivers
- Các dự án "open AUTOSAR" trên GitHub để xem cấu trúc BSW

## Lộ trình nghề nghiệp gợi ý
1. **Junior Embedded/Firmware Engineer:** Embedded C, MCU, driver, debug, Git, đọc datasheet.
2. **Automotive SW Engineer:** + CAN/UDS, AUTOSAR config (DaVinci/EB tresos), MISRA, unit test, ASPICE.
3. **Senior / Specialist:** chọn hướng sâu – BSW/MCAL, Diagnostics/Bootloader, Functional Safety, Cybersecurity, Communication (Ethernet/SOME/IP), Motor control/BMS, Adaptive AUTOSAR/Linux.
4. **Architect / Safety Manager / Technical Lead.**
