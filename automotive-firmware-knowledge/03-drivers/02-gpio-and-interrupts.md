# 02 – GPIO & Interrupt

## 1. Cấu hình một chân (Port)
| Thuộc tính | Lựa chọn |
|---|---|
| Mode | Input / Output / Alternate function (UART, SPI…) / Analog |
| Output type | Push-pull / Open-drain |
| Pull | None / Pull-up / Pull-down |
| Speed / slew rate / drive strength | Low (giảm EMC) → High |
| Input filter | Digital filter, Schmitt trigger |
| Interrupt | Rising / Falling / Both / Level |
| Initial value | Mức ban đầu khi chuyển sang output (tránh glitch kích nhầm tải) |

AUTOSAR tách làm **Port** (cấu hình chân, `Port_Init`) và **Dio** (đọc/ghi: `Dio_ReadChannel`, `Dio_WriteChannel`, `Dio_FlipChannel`, `Dio_ReadPort`, `Dio_WriteChannelGroup`).

## 2. Driver GPIO bare-metal (STM32F4 ví dụ)
```c
typedef enum { GPIO_IN = 0, GPIO_OUT = 1, GPIO_AF = 2, GPIO_AN = 3 } GpioMode_t;

void Gpio_Init(GPIO_TypeDef *port, uint8_t pin, GpioMode_t mode, uint8_t pull)
{
    port->MODER = (port->MODER & ~(3UL << (pin * 2u))) | ((uint32_t)mode << (pin * 2u));
    port->PUPDR = (port->PUPDR & ~(3UL << (pin * 2u))) | ((uint32_t)pull << (pin * 2u));
}

static inline void Gpio_Write(GPIO_TypeDef *port, uint8_t pin, bool level)
{
    /* BSRR: bit[15:0] set, bit[31:16] reset → atomic, không cần RMW */
    port->BSRR = level ? (1UL << pin) : (1UL << (pin + 16u));
}

static inline bool Gpio_Read(const GPIO_TypeDef *port, uint8_t pin)
{
    return ((port->IDR >> pin) & 1UL) != 0u;
}
```
Nhớ bật clock: `RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;`

## 3. Interrupt – cơ chế

```
Sự kiện phần cứng → Peripheral set cờ (pending) → Interrupt controller (NVIC)
→ kiểm tra enable + priority → CPU lưu context → nhảy vector → ISR
→ ISR clear cờ → return → CPU khôi phục context
```

### NVIC (Cortex-M)
```c
NVIC_SetPriority(EXTI15_10_IRQn, 5);   /* số nhỏ = ưu tiên cao */
NVIC_EnableIRQ(EXTI15_10_IRQn);
```
- Priority grouping: preempt priority vs sub-priority.
- **BASEPRI**: chặn ngắt có priority thấp hơn ngưỡng (FreeRTOS dùng `configMAX_SYSCALL_INTERRUPT_PRIORITY`).
- **PRIMASK**: chặn tất cả (trừ NMI, HardFault).

### Interrupt controller khác
- TriCore: **SRC (Service Request Control)** + ICU, mỗi nguồn có SRPN (priority) và TOS (core đích / DMA).
- RH850: **INTC1/INTC2**, EI level, table reference.

## 4. External interrupt cho nút nhấn
```c
void EXTI15_10_IRQHandler(void)
{
    if ((EXTI->PR & EXTI_PR_PR13) != 0u) {
        EXTI->PR = EXTI_PR_PR13;            /* W1C – clear cờ */
        s_buttonEvent = true;               /* xử lý ở task */
    }
}
```
⚠️ Nút cơ khí bounce → nhiều ngắt. Với automotive, input công tắc thường được **lấy mẫu chu kỳ (polling 5–10 ms) + debounce phần mềm** thay vì ngắt cạnh. Ngắt cạnh dùng cho wake-up hoặc tín hiệu nhanh.

## 5. Quy tắc viết ISR
1. Ngắn – đo bằng GPIO toggle.
2. Clear cờ đúng cách (W1C, đọc data register để clear…).
3. Biến chia sẻ: `volatile` + bảo vệ khi đọc nhiều byte.
4. Không gọi hàm blocking, không chờ cờ khác lâu.
5. Không gọi hàm không reentrant (strtok, printf, malloc).
6. Xử lý **tất cả** nguồn ngắt có thể (ISR dùng chung cho nhiều cờ).
7. Có handler mặc định cho ngắt không mong đợi → ghi lỗi.
8. Xem xét **interrupt storm** (nhiễu liên tục gây ngắt) → giới hạn tần suất, tự disable.

## 6. Wake-up
GPIO wake-up từ sleep (KL15, nút khoá cửa, CAN transceiver INH/wake). Khi thức dậy, firmware xác định **wake-up source** (EcuM_CheckWakeup / EcuM_ValidateWakeupEvent) để quyết định khởi động đầy đủ hay ngủ lại.

---

## Câu hỏi tự kiểm tra
1. Tại sao dùng BSRR thay vì `ODR |= ...`?
2. Preempt priority và sub-priority khác nhau thế nào?
3. Nút nhấn trên xe nên dùng ngắt hay polling? Tại sao?
4. Điều gì xảy ra nếu ISR không clear cờ?

## Thực hành
- Viết driver GPIO register-level (không HAL), toggle LED bằng nút qua EXTI.
- Đo interrupt latency: input từ function generator → ISR toggle output → đo độ trễ trên scope.
