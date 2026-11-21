# 04 – UART, SPI, I2C

| | UART | SPI | I2C |
|---|---|---|---|
| Dây | TX, RX (+GND) | SCK, MOSI, MISO, CS | SDA, SCL |
| Đồng bộ | Không (baud cố định) | Có clock | Có clock |
| Tốc độ | ≤ vài Mbps | Vài–vài chục MHz | 100k / 400k / 1M |
| Topology | Point-to-point | 1 master, nhiều slave (CS riêng) | Multi-master, địa chỉ 7/10-bit |
| Dùng trong ECU | Debug log, LIN (dựa UART) | **SBC, EEPROM, smart switch, IMU, ADC ngoài, motor driver** | Ít (EEPROM, sensor) – I2C kém robust trong môi trường nhiễu |

## 1. UART

Khung: `Start(0) | D0..D7 | [Parity] | Stop(1)` – thường **8N1**.
Sai số baud: tổng sai số TX+RX nên < ~2–3%.

```c
void Uart_Init(uint32_t pclk, uint32_t baud)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    USART2->BRR = (pclk + baud / 2u) / baud;          /* oversampling 16, làm tròn */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;
    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void)
{
    uint32_t sr = USART2->SR;
    if ((sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE)) != 0u) {
        (void)USART2->DR;                 /* đọc SR rồi DR để clear lỗi */
        s_uartErrCnt++;
    } else if ((sr & USART_SR_RXNE) != 0u) {
        (void)rb_put(&s_rx, (uint8_t)USART2->DR);
    } else { /* không làm gì */ }
}
```
- Lỗi: **Overrun** (không đọc kịp), **Framing** (baud sai), **Noise**, **Parity**.
- Nhận khung dài: DMA + **IDLE line interrupt**.
- Gửi: ring buffer TX + ngắt TXE, không blocking.

**LIN** dùng UART + break detection (LIN mode trong LPUART/LINFlex).

## 2. SPI

4 mode theo **CPOL** (mức nghỉ clock) và **CPHA** (cạnh lấy mẫu):
| Mode | CPOL | CPHA | Lấy mẫu |
|---|---|---|---|
| 0 | 0 | 0 | Cạnh lên |
| 1 | 0 | 1 | Cạnh xuống |
| 2 | 1 | 0 | Cạnh xuống |
| 3 | 1 | 1 | Cạnh lên |

→ Đọc datasheet slave để chọn mode, tốc độ tối đa, thứ tự bit (MSB first), độ dài frame (8/16/32 bit), thời gian CS setup/hold.

```c
uint8_t Spi_Transfer(uint8_t tx)
{
    uint32_t to = SPI_TIMEOUT;
    while (((SPI1->SR & SPI_SR_TXE) == 0u) && (--to > 0u)) { }
    SPI1->DR = tx;
    to = SPI_TIMEOUT;
    while (((SPI1->SR & SPI_SR_RXNE) == 0u) && (--to > 0u)) { }
    return (uint8_t)SPI1->DR;
}

/* Ghi register của SBC: frame 16 bit [R/W | addr(7) | data(8)] */
void Sbc_WriteReg(uint8_t addr, uint8_t data)
{
    CS_LOW();
    (void)Spi_Transfer((uint8_t)(0x80u | (addr & 0x7Fu)));
    (void)Spi_Transfer(data);
    CS_HIGH();
}
```
- Nhiều IC automotive dùng frame có **CRC** và trả về **status/fault byte** trong mỗi frame → driver phải kiểm tra.
- Daisy-chain: nhiều IC chung CS, dữ liệu dịch qua nhau.

**AUTOSAR SPI handler/driver:** khái niệm **Channel → Job (1 CS) → Sequence**; `Spi_SetupEB`, `Spi_AsyncTransmit`, `Spi_SyncTransmit`, `Spi_GetSequenceResult`. Level 0/1/2 (sync/async/cả hai).

## 3. I2C
- Open-drain + pull-up. Khung: `START | ADDR(7)+R/W | ACK | DATA | ACK | ... | STOP`. Repeated START cho read-after-write.
- Clock stretching (slave giữ SCL thấp).
- **Bus lock-up:** slave giữ SDA thấp sau reset giữa chừng → khôi phục bằng cách toggle SCL 9 lần rồi tạo STOP.
- Luôn có timeout ở mỗi bước, kiểm tra NACK.

```c
/* Ví dụ ghi EEPROM 24C02 tại word addr */
Std_ReturnType Eep_WriteByte(uint8_t mem, uint8_t val)
{
    if (I2c_Start() != E_OK)                 { return E_NOT_OK; }
    if (I2c_SendAddr(0x50u, I2C_WRITE) != E_OK) { I2c_Stop(); return E_NOT_OK; }
    (void)I2c_SendByte(mem);
    (void)I2c_SendByte(val);
    I2c_Stop();
    /* EEPROM cần ~5 ms write cycle: ACK polling */
    return E_OK;
}
```

---

## Câu hỏi tự kiểm tra
1. Baud 115200, PCLK 42 MHz, oversampling 16 → BRR = ? Sai số bao nhiêu?
2. UART overrun xảy ra khi nào? Cách tránh?
3. Chọn SPI mode cho IC yêu cầu "data latched on rising edge, SCK idle low".
4. Tại sao SPI phổ biến hơn I2C trong ECU?
5. Khôi phục I2C bus bị treo như thế nào?

## Thực hành
- Driver UART interrupt + ring buffer, viết command-line nhỏ (`help`, `led on`, `adc`).
- SPI đọc ID của một IC (ví dụ IMU MPU6500 / flash W25Q / MCP2515 CAN controller), kiểm tra bằng logic analyzer.
- I2C đọc/ghi EEPROM 24Cxx với ACK polling.
