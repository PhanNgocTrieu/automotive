# 01 – AUTOSAR BSW / MCAL

## Vai trò công việc
**Chức danh:** AUTOSAR Engineer, BSW Engineer, MCAL Developer, Integration Engineer.

**Công việc hằng ngày:**
- Cấu hình các module BSW (Com, Dcm, Dem, NvM, OS, EcuM, BswM…) bằng DaVinci Configurator hoặc EB tresos
- Tích hợp SWC của application với RTE, giải quyết lỗi build/link/runtime
- Viết hoặc port MCAL cho MCU mới, viết Complex Device Driver (CDD)
- Phân tích timing, memory, sửa lỗi tích hợp trên target

## Phù hợp nếu bạn
Thích driver, register, kiến trúc phần mềm phân lớp; kiên nhẫn đọc spec dài.

## Điều kiện tiên quyết
- [04-autosar](../../automotive-firmware-knowledge/01-automotive-fundamentals/04-autosar.md), [01-driver-design-principles](../../automotive-firmware-knowledge/03-drivers/01-driver-design-principles.md), [04-rtos-and-osek](../../automotive-firmware-knowledge/02-firmware/04-rtos-and-osek.md)
- Đã làm Lab A9 (S32K144 + RTD MCAL)

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | AUTOSAR methodology, ARXML, ECU extract, ECUC | 2 |
| 02 | MCAL chuyên sâu: Mcu, Port, Dio, Adc, Pwm, Icu, Gpt, Spi, Fls, Wdg – đọc SWS, cấu hình RTD | 4 |
| 03 | Communication stack: Can, CanIf, PduR, Com, CanTp, CanSM, ComM, CanNm, CanTrcv | 4 |
| 04 | Memory stack: NvM, MemIf, Fee, Ea, Fls | 2 |
| 05 | Diagnostic stack: Dcm, Dem, FiM, Det | 2 |
| 06 | System services: AUTOSAR OS (SC1–SC4), EcuM, BswM, WdgM, SchM | 3 |
| 07 | RTE: SWC, port, interface, runnable, RTE event, mapping lên OS task | 2 |
| 08 | Complex Device Driver & tích hợp IC ngoài (SBC, smart switch) | 2 |
| 09 | Multi-core AUTOSAR, timing analysis, memory partitioning | 3 |

**Danh sách bài cần viết:**
- [ ] `01-autosar-methodology-arxml.md`
- [ ] `02-mcal-deep-dive.md`
- [ ] `03-communication-stack.md`
- [ ] `04-memory-stack.md`
- [ ] `05-diagnostic-stack.md`
- [ ] `06-os-ecum-bswm-wdgm.md`
- [ ] `07-rte-and-swc.md`
- [ ] `08-complex-device-driver.md`
- [ ] `09-multicore-timing.md`

## Tài liệu
- AUTOSAR Classic Platform specifications (miễn phí, autosar.org): SWS_Can, SWS_CanIf, SWS_Com, SWS_PduR, SWS_NvM, SWS_Os, SWS_Rte, *Layered Software Architecture*
- *Automotive Software Architectures* – Miroslaw Staron
- Vector E-Learning: AUTOSAR

## Công cụ
| Miễn phí / để học | Thương mại (gặp khi đi làm) |
|---|---|
| NXP S32 Design Studio + RTD (MCAL), Infineon iLLD | Vector DaVinci Developer/Configurator + MICROSAR |
| ARXML editor (Artop – Eclipse) | Elektrobit EB tresos + AutoCore |
| | ETAS ISOLAR + RTA-CAR, Lauterbach TRACE32 |

## Dự án thực hành
1. **MCAL bring-up:** cấu hình Port/Dio/Adc/Pwm/Can bằng RTD trên S32K144, đọc ADC và gửi giá trị qua CAN.
2. **Mini AUTOSAR stack tự viết:** CanIf → PduR → Com theo đúng API SWS, sinh cấu hình từ DBC bằng script Python.
3. **CDD cho SBC:** driver SPI cho FS65/UJA1169, có watchdog servicing và quản lý mode.

## Mốc hoàn thành
- [ ] Giải thích được mọi module trong sơ đồ Layered Architecture và API chính
- [ ] Trace được một CAN signal từ ARXML/DBC qua toàn bộ stack đến `Rte_Read`
- [ ] Hoàn thành ít nhất 2 trong 3 dự án

## Hướng tiếp theo
Kết hợp với [03 Functional Safety](../03-functional-safety/) (OS-Application, WdgM, E2E) hoặc [05 Networks](../05-vehicle-networks-ethernet/) (Eth stack, SoAd, SOME/IP).
