# 02 – Mạng truyền thông trên xe (In-Vehicle Networks)

## 1. Tổng quan

| Mạng | Tốc độ | Topology | Ứng dụng | Chi phí |
|---|---|---|---|---|
| LIN | ≤ 20 kbps | Master/slave, 1 dây | Gương, ghế, cửa sổ, cảm biến mưa | Rất thấp |
| CAN (HS) | ≤ 1 Mbps (thường 500k) | Bus, multi-master | Powertrain, chassis, body | Thấp |
| CAN FD | Arbitration 500k–1M, data 2–8 Mbps | Bus | Thay CAN, flashing nhanh hơn | Thấp |
| CAN XL | Data ≤ 20 Mbps, payload 2048 B | Bus | Thế hệ mới | |
| FlexRay | 10 Mbps, deterministic (TDMA) | Bus/star | Chassis, x-by-wire (đang giảm) | Cao |
| Automotive Ethernet (100/1000BASE-T1) | 100 Mbps – 10 Gbps | Switch, point-to-point | Backbone, ADAS, camera, OTA | Trung bình |
| 10BASE-T1S | 10 Mbps multi-drop | Bus | Thay CAN ở zonal | Thấp |
| SENT / PSI5 | Thấp | Point-to-point | Cảm biến | Thấp |

## 2. CAN (Controller Area Network) – ISO 11898

### 2.1 Lớp vật lý
- 2 dây vi sai **CAN_H**, **CAN_L**, xoắn đôi, **termination 120Ω ở 2 đầu** (đo giữa H–L khi tắt nguồn ≈ 60Ω).
- **Recessive (logic 1):** CAN_H ≈ CAN_L ≈ 2.5V (chênh ~0V).
- **Dominant (logic 0):** CAN_H ≈ 3.5V, CAN_L ≈ 1.5V (chênh ~2V).
- Dominant "thắng" recessive → cơ chế **wired-AND** → nền tảng của arbitration.

### 2.2 Khung dữ liệu (Classic CAN, standard 11-bit ID)
```
| SOF | ID (11) | RTR | IDE | r0 | DLC (4) | DATA (0–8 B) | CRC (15) | DEL | ACK | DEL | EOF (7) | IFS |
```
- **ID:** cũng là độ ưu tiên – ID **nhỏ hơn** ưu tiên **cao hơn**.
- **Extended frame:** 29-bit ID (dùng trong J1939 cho xe tải).
- **DLC:** số byte data (0–8). CAN FD: tới 64 byte (DLC 9–15 mã hoá 12,16,20,24,32,48,64).
- **Bit stuffing:** sau 5 bit giống nhau chèn 1 bit ngược lại để đồng bộ.
- **ACK:** bất kỳ node nào nhận đúng đều kéo ACK slot dominant.

### 2.3 Arbitration (CSMA/CR – non-destructive)
Nhiều node gửi cùng lúc, so từng bit ID. Node gửi recessive nhưng đọc lại dominant → mất quyền, chuyển sang nhận. Frame ưu tiên cao không bị trễ.

### 2.4 Bit timing
```
1 bit = Sync_Seg (1 tq) + Prop_Seg + Phase_Seg1 + Phase_Seg2
Sample point ~ 75–87.5% (automotive thường 80% hoặc 87.5%)
Baud = f_can_clk / (prescaler × số_tq_mỗi_bit)
```
Ví dụ: f = 40 MHz, prescaler = 5 → tq = 125 ns; 16 tq/bit → 500 kbps; Seg1 = 13, Seg2 = 2 → sample point = (1+13)/16 = 87.5%. SJW = 2.

### 2.5 Xử lý lỗi
5 loại lỗi: Bit, Stuff, CRC, Form, ACK.
Mỗi node có **TEC** (Transmit Error Counter) và **REC**:
```
Error Active (TEC,REC < 128) → Error Passive (≥128) → Bus Off (TEC ≥ 256)
```
Bus-off recovery: sau 128 × 11 bit recessive; trong AUTOSAR do **CanSM** quản lý (quick recovery rồi slow recovery, kèm DTC).

### 2.6 Tầng trên CAN
- **DBC file:** định nghĩa message (ID, DLC, chu kỳ) và signal (start bit, length, byte order Intel/Motorola, factor, offset, đơn vị, min/max).
  ```
  BO_ 256 EngineData: 8 ECM
   SG_ EngineSpeed : 0|16@1+ (0.25,0) [0|16383.75] "rpm" Cluster
   SG_ CoolantTemp : 16|8@1+ (1,-40) [-40|215] "degC" Cluster
  ```
  Physical = raw × factor + offset → EngineSpeed raw 0x0FA0 = 4000 × 0.25 = 1000 rpm.
- **ISO-TP (ISO 15765-2):** chia dữ liệu > 8 byte thành nhiều frame: Single Frame, First Frame, Consecutive Frame, Flow Control. Dùng cho UDS.
- **J1939:** xe tải/bus, 29-bit ID có PGN.
- **CANopen:** công nghiệp (ít trong ô tô con).
- **E2E protection (AUTOSAR E2E):** thêm **CRC + alive counter** vào signal safety để phát hiện mất/lặp/sai thứ tự/hỏng message.

### 2.7 Network management
**AUTOSAR CanNm / OSEK NM:** ECU gửi NM message để giữ mạng thức; khi tất cả không cần nữa → cả mạng cùng ngủ (**coordinated sleep**). Partial networking: chỉ một nhóm ECU thức.

## 3. LIN (Local Interconnect Network) – ISO 17987
- 1 dây, 12V level, dựa trên UART (8N1), ≤ 20 kbps.
- **1 master, tối đa 16 slave.** Master gửi **header** (Break + Sync 0x55 + PID), slave tương ứng gửi **response** (data 1–8 B + checksum).
- Lịch truyền: **schedule table** cố định trong master.
- Mô tả mạng: **LDF file**.
- Slave thường không cần thạch anh: tự đồng bộ baud qua Sync field.

## 4. FlexRay
- 2 kênh A/B, 10 Mbps, **TDMA** (static segment) + dynamic segment → deterministic, fault-tolerant.
- Phức tạp, chi phí cao, đang được thay thế bởi Ethernet + CAN FD.

## 5. Automotive Ethernet
- **100BASE-T1 / 1000BASE-T1:** 1 cặp dây xoắn, full-duplex.
- Giao thức: **SOME/IP** (service-oriented middleware), **SOME/IP-SD**, **DoIP (ISO 13400)** cho chẩn đoán qua Ethernet, **AVB/TSN** (time-sensitive), **gPTP** (đồng bộ thời gian), **DDS**.
- Bảo mật: **MACsec**, TLS/IPsec.

## 6. Công cụ
| Tool | Ghi chú |
|---|---|
| Vector CANoe / CANalyzer | Chuẩn công nghiệp, đắt |
| PCAN-View, Kvaser CanKing, BusMaster | Rẻ/miễn phí |
| **Linux SocketCAN + can-utils** | `candump`, `cansend`, `cangen`, `isotpsend`… miễn phí |
| python-can, cantools | Script, decode DBC |
| Wireshark | Ethernet, SOME/IP, DoIP, cũng đọc được SocketCAN |

---

## Câu hỏi tự kiểm tra
1. Hai node gửi ID 0x123 và 0x120 cùng lúc, ai thắng? Giải thích từng bit.
2. Đo điện trở CAN_H–CAN_L được 120Ω. Có vấn đề gì?
3. Tính bit timing cho 500 kbps với clock 80 MHz, sample point 80%.
4. Giải thích Error Passive và Bus Off.
5. Vì sao UDS cần ISO-TP?
6. Decode tín hiệu Motorola: start bit 7, length 16, data `0x12 0x34 ...`.

## Thực hành (Linux, không cần phần cứng)
```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
candump vcan0 &
cansend vcan0 123#DEADBEEF
cangen vcan0 -g 10 -I 100 -L 8
```
- Viết DBC nhỏ, dùng `python-can` + `cantools` để encode/decode signal.
- Với phần cứng: 2 board STM32 + transceiver, gửi message chu kỳ 10ms, đo bằng logic analyzer (decode CAN).
