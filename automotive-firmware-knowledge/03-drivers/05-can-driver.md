# 05 – CAN Driver

Đọc trước: `01-automotive-fundamentals/02-in-vehicle-networks.md`.

## 1. Phần cứng
```
MCU [CAN controller: bxCAN / FDCAN / FlexCAN / MCAN / MultiCAN] ── TX/RX (logic) ── [Transceiver TJA1043/TJA1145] ── CAN_H/CAN_L
```
- **Controller:** framing, bit timing, arbitration, CRC, error counter, mailbox/FIFO, filter.
- **Transceiver:** chuyển logic ↔ vi sai, chế độ Normal / Standby / Sleep, wake-up, bảo vệ bus. Một số có SPI (partial networking, TJA1145).

## 2. Mailbox, FIFO, Filter
- **TX mailbox / buffer:** đặt frame vào, controller tự arbitration. Nhiều mailbox → chú ý **priority inversion nội bộ** (mailbox gửi theo ID hay theo thứ tự?).
- **RX FIFO / RX buffer:** nhận frame.
- **Acceptance filter:** chỉ nhận ID quan tâm (mask/list mode) → giảm tải CPU.
- AUTOSAR: **HTH** (Hardware Transmit Handle), **HRH** (Hardware Receive Handle), BasicCAN (FIFO, nhiều ID) vs FullCAN (1 ID / mailbox).

## 3. Init sequence (chung cho hầu hết controller)
1. Bật clock CAN, chọn nguồn clock (nên dùng clock từ thạch anh, không dùng RC nội vì sai số lớn).
2. Cấu hình chân TX/RX (alternate function).
3. Vào **init/freeze mode**.
4. Cấu hình bit timing (prescaler, seg1, seg2, SJW) – CAN FD thêm data phase + TDC (transmitter delay compensation).
5. Cấu hình filter, mailbox.
6. Bật ngắt (RX, TX complete, error, bus-off).
7. Thoát init mode → đợi đồng bộ bus (11 bit recessive).
8. Đưa transceiver vào Normal mode (chân STB/EN).

## 4. API ví dụ
```c
typedef struct {
    uint32_t id;       /* bit31 = extended flag */
    uint8_t  dlc;
    uint8_t  data[8];
} CanFrame_t;

Std_ReturnType Can_Init(uint32_t bitrate);
Std_ReturnType Can_Write(const CanFrame_t *f);      /* non-blocking: E_NOT_OK nếu mailbox bận (CAN_BUSY) */
void           Can_RxIsr(void);                       /* đọc FIFO → callback */
void           Can_MainFunction_BusOff(void);
extern void    CanIf_RxIndication(const CanFrame_t *f);   /* callback lên tầng trên */
extern void    CanIf_TxConfirmation(uint32_t id);
```

Ví dụ (STM32 bxCAN):
```c
Std_ReturnType Can_Write(const CanFrame_t *f)
{
    uint32_t tsr = CAN1->TSR;
    uint32_t mb;

    if ((tsr & CAN_TSR_TME0) != 0u)      { mb = 0u; }
    else if ((tsr & CAN_TSR_TME1) != 0u) { mb = 1u; }
    else if ((tsr & CAN_TSR_TME2) != 0u) { mb = 2u; }
    else { return E_NOT_OK; }            /* CAN_BUSY */

    CAN1->sTxMailBox[mb].TIR  = (f->id & 0x7FFu) << 21;           /* standard ID */
    CAN1->sTxMailBox[mb].TDTR = f->dlc & 0xFu;
    CAN1->sTxMailBox[mb].TDLR = (uint32_t)f->data[0]       | ((uint32_t)f->data[1] << 8) |
                                ((uint32_t)f->data[2] << 16) | ((uint32_t)f->data[3] << 24);
    CAN1->sTxMailBox[mb].TDHR = (uint32_t)f->data[4]       | ((uint32_t)f->data[5] << 8) |
                                ((uint32_t)f->data[6] << 16) | ((uint32_t)f->data[7] << 24);
    CAN1->sTxMailBox[mb].TIR |= CAN_TI0R_TXRQ;                    /* request transmit */
    return E_OK;
}
```

## 5. Xử lý lỗi & Bus-off
- Đọc TEC/REC, trạng thái Error Warning / Passive / Bus-off từ register.
- **Bus-off recovery** (AUTOSAR CanSM):
  - Quick recovery: thử lại sau L1 = 10 ms, tối đa N lần.
  - Slow recovery: sau đó mỗi L2 = 1000 ms.
  - Báo DTC "CAN bus-off" sau ngưỡng.
- Tắt auto bus-off recovery của phần cứng nếu muốn phần mềm kiểm soát.

## 6. Tầng trên driver (tự viết hoặc AUTOSAR)
```
Can driver ─► CanIf (dispatch theo ID → PduId) ─► PduR ─► Com (signal) / CanTp (ISO-TP) ─► Dcm
```
- **Com:** quản lý gửi chu kỳ (cyclic / on-change / mixed), **Rx timeout (deadline monitoring)** → giá trị thay thế + DTC "lost communication with ECU X" (U-code).
- **Signal packing:** theo DBC (Intel = little-endian, Motorola = big-endian).

```c
/* Pack signal Intel, start bit sb, length len vào buffer 8 byte */
void Sig_PackIntel(uint8_t *buf, uint8_t sb, uint8_t len, uint32_t raw)
{
    for (uint8_t i = 0u; i < len; i++) {
        uint8_t bit = (uint8_t)(sb + i);
        if (((raw >> i) & 1u) != 0u) { buf[bit / 8u] |=  (uint8_t)(1u << (bit % 8u)); }
        else                         { buf[bit / 8u] &= (uint8_t)~(1u << (bit % 8u)); }
    }
}
```

## 7. Kiểm thử CAN driver
- Loopback mode / silent mode của controller (test không cần bus).
- Gửi/nhận với PC (CANable + SocketCAN) – `candump -td can0`.
- Đo chu kỳ & jitter bằng `candump -td` hoặc CANoe.
- Rút dây/chập CAN_H-CAN_L → kiểm tra bus-off & recovery.
- Busload test: `cangen can0 -g 0` để tạo tải cao, kiểm tra không mất frame (overrun FIFO).

---

## Câu hỏi tự kiểm tra
1. BasicCAN và FullCAN khác nhau?
2. Tại sao không nên dùng RC oscillator nội làm clock CAN?
3. Mô tả chiến lược bus-off recovery.
4. Rx timeout của một message được xử lý ở tầng nào và như thế nào?
5. Pack signal Motorola khác Intel ra sao?

## Thực hành
- Viết CAN driver register-level cho STM32 (bxCAN/FDCAN) hoặc S32K (FlexCAN): gửi frame 100 ms, nhận và in ra UART.
- Viết module "Com mini": bảng message từ DBC, gửi chu kỳ, timeout nhận.
- Kết nối với PC qua CANable, dùng `cantools` decode.
