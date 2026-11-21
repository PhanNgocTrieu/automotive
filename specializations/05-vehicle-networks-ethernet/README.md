# 05 – Vehicle Networks & Automotive Ethernet

## Vai trò công việc
**Chức danh:** Communication Engineer, Network Architect, Gateway Developer, Ethernet/SOME/IP Engineer.

**Công việc hằng ngày:**
- Thiết kế và tích hợp communication matrix (DBC, LDF, ARXML, FIBEX)
- Phát triển gateway ECU: định tuyến CAN ↔ CAN FD ↔ Ethernet
- Cấu hình Ethernet stack (TcpIp, SoAd, SOME/IP, DoIP), switch, VLAN, TSN
- Phân tích busload, timing, lỗi mạng bằng CANoe/Wireshark

## Phù hợp nếu bạn
Thích giao thức, mạng máy tính, phân tích log và tối ưu hiệu năng.

## Điều kiện tiên quyết
- [02-in-vehicle-networks](../../automotive-firmware-knowledge/01-automotive-fundamentals/02-in-vehicle-networks.md), [05-can-driver](../../automotive-firmware-knowledge/03-drivers/05-can-driver.md)
- Kiến thức mạng máy tính cơ bản: OSI, Ethernet, IP, TCP/UDP

## Đề cương

| Module | Nội dung | Tuần |
|---|---|---|
| 01 | CAN FD & CAN XL chuyên sâu: bit timing 2 pha, TDC, ringing, thiết kế topology | 2 |
| 02 | LIN chuyên sâu: schedule table, diagnostic frame, node configuration | 1 |
| 03 | Network management: CanNm, partial networking, wake-up, coordinated sleep | 2 |
| 04 | Automotive Ethernet PHY: 100/1000BASE-T1, 10BASE-T1S, switch, VLAN | 3 |
| 05 | TCP/IP stack cho MCU (lwIP), AUTOSAR TcpIp/SoAd | 2 |
| 06 | SOME/IP, SOME/IP-SD, serialization; DDS (so sánh) | 3 |
| 07 | TSN & gPTP: đồng bộ thời gian, traffic shaping | 2 |
| 08 | Gateway & zonal architecture: routing, PDU gateway, signal gateway, firewall | 3 |
| 09 | Phân tích & kiểm thử mạng: busload, latency, conformance test | 2 |

**Danh sách bài cần viết:**
- [ ] `01-can-fd-xl-deep-dive.md`
- [ ] `02-lin-deep-dive.md`
- [ ] `03-network-management.md`
- [ ] `04-automotive-ethernet-phy.md`
- [ ] `05-tcpip-on-mcu.md`
- [ ] `06-someip.md`
- [ ] `07-tsn-gptp.md`
- [ ] `08-gateway-zonal.md`
- [ ] `09-network-analysis-testing.md`

## Tài liệu
- ISO 11898-1/2, ISO 17987, IEEE 802.3bw/bp/cg, IEEE 802.1AS/Qbv
- AUTOSAR SOME/IP Protocol Specification, SWS_SoAd, SWS_CanNm
- *Automotive Ethernet* – Kirsten Matheus & Thomas Königseder

## Công cụ
Wireshark, vsomeip (COVESA), lwIP, SocketCAN, SavvyCAN; Vector CANoe/CANalyzer, Technica Engineering media converter (thương mại).

## Dự án thực hành
1. **CAN ↔ CAN gateway** trên MCU có 2 controller: bảng routing, lọc ID, đo latency.
2. **SOME/IP service** giữa Linux (vsomeip) và MCU (lwIP + SOME/IP tự viết), có service discovery.
3. **Zonal demo:** 2 zone ECU (MCU) gom I/O và gửi lên "central compute" (Raspberry Pi) qua Ethernet.

## Mốc hoàn thành
- [ ] Tính và đo được busload, đề xuất chia lại message cho mạng quá tải
- [ ] Đọc và giải thích được một capture SOME/IP trên Wireshark
- [ ] Hoàn thành ít nhất 2 trong 3 dự án

## Hướng tiếp theo
Kết hợp với [07 Adaptive/HPC](../07-adaptive-autosar-hpc/) hoặc [04 Cybersecurity](../04-cybersecurity/) (MACsec, IDS).
