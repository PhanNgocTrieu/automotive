# 01 – Nguyên tắc thiết kế Driver

## 1. Driver là gì?
Lớp phần mềm **trực tiếp điều khiển phần cứng** (register), cung cấp API trừu tượng cho tầng trên. Tầng trên không cần biết register, chỉ gọi `Adc_StartGroupConversion()`, `Can_Write()`...

## 2. Các tầng
```
Application
    │  (không biết phần cứng)
ECU Abstraction / IoHwAb    ← "Đèn pha", "Nhiệt độ nước" → kênh ADC/DIO nào
    │
On-board device drivers     ← SBC, EEPROM ngoài, smart switch (qua SPI)
    │
MCAL (Microcontroller Abstraction Layer) ← GPIO, ADC, CAN... của chip
    │
Registers
```
- **MCAL:** phụ thuộc chip, thường do **hãng chip cung cấp** (NXP RTD, Infineon iLLD/MCAL, Renesas MCAL) — được qualify ASIL.
- **HAL (vendor)** như STM32 HAL, LL: tương tự nhưng không theo AUTOSAR.
- Firmware engineer thường: cấu hình MCAL, viết **CDD** (Complex Device Driver), viết driver cho IC ngoài, hoặc viết MCAL cho chip mới (ở công ty bán MCAL).

## 3. Mẫu API driver (theo phong cách AUTOSAR)

```c
/* Xxx.h */
typedef struct {
    uint32_t baudrate;
    uint8_t  channel;
    /* ... */
} Xxx_ConfigType;

void           Xxx_Init(const Xxx_ConfigType *ConfigPtr);
void           Xxx_DeInit(void);
Std_ReturnType Xxx_Write(uint8_t ch, const uint8_t *data, uint16_t len);
Std_ReturnType Xxx_Read(uint8_t ch, uint8_t *data, uint16_t len);
void           Xxx_MainFunction(void);                 /* gọi định kỳ khi polling */
void           Xxx_GetVersionInfo(Std_VersionInfoType *v);
```

Nguyên tắc:
1. **Cấu hình tách khỏi code** (`Xxx_Cfg.h`, `Xxx_PBcfg.c` – thường do tool generate). Cùng một code driver dùng cho nhiều ECU.
2. **Kiểm tra lỗi phát triển (DET):** tham số NULL, chưa init, channel ngoài dải → `Det_ReportError()` khi `XXX_DEV_ERROR_DETECT == STD_ON`.
3. **Trạng thái module:** UNINIT → INIT; gọi API khi chưa init → lỗi.
4. **Không blocking vô hạn:** mọi vòng chờ cờ phần cứng phải có **timeout**.
5. **Hỗ trợ polling và interrupt** (cấu hình được).
6. **Callback/notification** lên tầng trên: `Xxx_Notification()`, `CanIf_RxIndication()`.
7. **Reentrancy:** ghi rõ API nào reentrant, bảo vệ vùng găng bằng `SchM_Enter/Exit`.
8. **Không cấp phát động.**
9. **Phân tách ISR và xử lý.**

## 4. Truy cập register
```c
/* Cách 1: struct overlay (CMSIS style) */
typedef struct {
    volatile uint32_t MODER;    /* 0x00 */
    volatile uint32_t OTYPER;   /* 0x04 */
    volatile uint32_t OSPEEDR;  /* 0x08 */
    volatile uint32_t PUPDR;    /* 0x0C */
    volatile uint32_t IDR;      /* 0x10 */
    volatile uint32_t ODR;      /* 0x14 */
    volatile uint32_t BSRR;     /* 0x18 */
} GPIO_TypeDef;
#define GPIOA ((GPIO_TypeDef *)0x40020000UL)

/* Cách 2: macro địa chỉ */
#define REG32(addr) (*(volatile uint32_t *)(addr))
```

Với safety, hay có hàm **đọc lại register sau khi ghi** (register readback / periodic register check) để phát hiện bit flip cấu hình.

## 5. Quy trình viết một driver mới
1. Đọc Reference Manual chương peripheral + errata.
2. Xác định yêu cầu: mode, tốc độ, interrupt/DMA, lỗi cần xử lý.
3. Thiết kế API & config struct.
4. Viết init sequence theo RM: **bật clock peripheral → reset peripheral → cấu hình pin (Port) → cấu hình peripheral → enable interrupt → enable peripheral**.
5. Viết chức năng đọc/ghi, ISR.
6. Xử lý lỗi (overrun, timeout, bus error).
7. Unit test (mock register) + test trên target bằng scope/logic analyzer.
8. Tài liệu: API, config, giới hạn, timing.

## 6. Một số lỗi kinh điển khi viết driver
- Quên bật clock peripheral → ghi register không có tác dụng (hoặc BusFault).
- Read-modify-write cờ W1C → mất cờ.
- Clear cờ ngắt quá muộn → ISR chạy lặp.
- Quên cấu hình alternate function cho chân.
- Không xử lý overrun (UART, ADC).
- Thứ tự init sai (enable peripheral trước khi cấu hình xong).
- Không có timeout khi chờ cờ → treo khi phần cứng lỗi.

---

## Câu hỏi tự kiểm tra
1. MCAL khác ECU Abstraction thế nào?
2. Vì sao tách config ra khỏi code driver?
3. Liệt kê thứ tự init chung của một peripheral.
4. Det dùng để làm gì? Có bật trong production không?
