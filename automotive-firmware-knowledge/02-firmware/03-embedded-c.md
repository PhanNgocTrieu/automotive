# 03 – Embedded C cho Automotive

## 1. Kiểu dữ liệu cố định
```c
#include <stdint.h>
#include <stdbool.h>
uint8_t  u8;   int16_t s16;   uint32_t u32;   bool flag;
```
`int` có thể 16 hoặc 32 bit tuỳ kiến trúc → không dùng cho dữ liệu có kích thước quan trọng.

## 2. Bit manipulation

```c
#define BIT(n)            (1UL << (n))
REG |=  BIT(3);           /* set bit 3     */
REG &= ~BIT(3);           /* clear bit 3   */
REG ^=  BIT(3);           /* toggle bit 3  */
if ((REG & BIT(3)) != 0u) /* test bit 3    */

/* Ghi trường nhiều bit: field [7:4] */
#define FIELD_MASK   (0xFUL << 4)
#define FIELD_POS    (4u)
REG = (REG & ~FIELD_MASK) | (((uint32_t)val << FIELD_POS) & FIELD_MASK);

/* Trích xuất */
uint32_t v = (REG & FIELD_MASK) >> FIELD_POS;
```

**Write-1-to-clear (W1C):**
```c
UART->SR = UART_SR_RXNE;      /* ✅ chỉ clear cờ RXNE */
UART->SR |= UART_SR_RXNE;     /* ❌ đọc rồi ghi lại → clear luôn các cờ khác đang = 1 */
```

**Bitfield struct:** tiện nhưng thứ tự bit & padding phụ thuộc compiler → tránh dùng để map register/giao thức nếu cần portable.

## 3. `volatile`
Báo compiler rằng giá trị có thể thay đổi ngoài luồng chương trình → không được tối ưu bỏ đọc/ghi.

Dùng khi:
1. Register ngoại vi.
2. Biến chia sẻ giữa ISR và main/task.
3. Biến chia sẻ giữa các core.

```c
static volatile bool s_rxDone = false;

void UART_IRQHandler(void) { s_rxDone = true; }

void wait(void) {
    while (!s_rxDone) { }     /* không có volatile → -O2 có thể thành vòng lặp vô hạn */
}
```
⚠️ `volatile` **không** đảm bảo atomic, không thay thế critical section/mutex.

## 4. Critical section & atomicity
```c
uint32_t primask = __get_PRIMASK();
__disable_irq();
/* --- đọc/ghi biến chia sẻ nhiều byte --- */
s_counter64++;
__set_PRIMASK(primask);    /* khôi phục trạng thái trước đó (an toàn khi lồng nhau) */
```
- Biến 32-bit trên MCU 32-bit, đọc/ghi aligned thường atomic; nhưng `x++` (read-modify-write) **không** atomic.
- Biến 64-bit hoặc struct → luôn cần bảo vệ.
- Trên Cortex-M3+: có `LDREX/STREX` cho lock-free; C11 `<stdatomic.h>`.
- Trong AUTOSAR: `SchM_Enter_<Module>_<Area>()` / `SchM_Exit_...`.

## 5. Con trỏ & const

```c
const uint8_t *p;        /* dữ liệu const, con trỏ đổi được */
uint8_t * const p;       /* con trỏ const */
const uint8_t * const p; /* cả hai */
volatile uint32_t * const REG = (volatile uint32_t *)0x40020014UL;
```
- Con trỏ hàm: callback, bảng xử lý trạng thái, bảng UDS service.
```c
typedef void (*Handler_t)(const uint8_t *data, uint16_t len);
static const Handler_t s_handlers[] = { svc10, svc22, svc27 };
```

## 6. Tràn số & scaling
```c
uint8_t a = 200, b = 100;
uint8_t s = a + b;           /* = 44 (tràn) */
uint16_t s2 = (uint16_t)a + b; /* = 300 */

/* Tính thời gian trôi qua với timer 32-bit wrap-around: unsigned subtraction luôn đúng */
uint32_t elapsed = now - start;      /* đúng kể cả khi now đã wrap */

/* Fixed-point: nhiệt độ 0.1°C */
int16_t temp_dC = (int16_t)(((int32_t)raw * 1650) / 4095) - 500;
```
- Nhân trước chia sau để giữ độ chính xác, nhưng cast lên kiểu rộng hơn để tránh tràn.
- Chú ý **integer promotion**: `uint8_t` → `int` khi tính toán.

## 7. Struct, packing, alignment
```c
typedef struct {
    uint8_t  id;      /* offset 0 */
                      /* padding 3 bytes */
    uint32_t value;   /* offset 4 */
} Msg_t;              /* sizeof = 8 */
```
- `__attribute__((packed))` giảm size nhưng truy cập unaligned có thể chậm hoặc HardFault (M0).
- Khi parse frame giao thức: **dùng shift/byte để ghép**, không ép kiểu con trỏ.
```c
uint16_t rpm = (uint16_t)((uint16_t)buf[0] << 8) | buf[1];   /* big-endian */
```

## 8. Macro vs inline
```c
#define SQUARE(x) ((x) * (x))       /* SQUARE(i++) → lỗi */
static inline int32_t square(int32_t x) { return x * x; }   /* ưu tiên */
```

## 9. Ring buffer (rất hay dùng: UART RX, log, event queue)
```c
#define RB_SIZE 64u   /* lũy thừa 2 */
typedef struct {
    uint8_t buf[RB_SIZE];
    volatile uint16_t head;   /* ghi bởi ISR (producer) */
    volatile uint16_t tail;   /* ghi bởi task (consumer) */
} RingBuf_t;

static bool rb_put(RingBuf_t *rb, uint8_t b) {
    uint16_t next = (uint16_t)((rb->head + 1u) & (RB_SIZE - 1u));
    if (next == rb->tail) { return false; }   /* full */
    rb->buf[rb->head] = b;
    rb->head = next;
    return true;
}
static bool rb_get(RingBuf_t *rb, uint8_t *b) {
    if (rb->tail == rb->head) { return false; } /* empty */
    *b = rb->buf[rb->tail];
    rb->tail = (uint16_t)((rb->tail + 1u) & (RB_SIZE - 1u));
    return true;
}
```
Single-producer/single-consumer → không cần khoá nếu head/tail được ghi atomic.

## 10. Những điều cấm/hạn chế trong automotive
- `malloc/free` (phân mảnh, không đoán định).
- Đệ quy (stack không tính trước được).
- `printf` trong ISR / production.
- Floating point trong ISR nếu không cần (lazy stacking FPU tốn thời gian) – nhiều dự án cấm float trong code safety.
- `goto` (MISRA advisory), trừ pattern cleanup có kiểm soát.
- Blocking delay vòng lặp (`for(i=0;i<100000;i++)`).

## 11. Pattern hữu ích
- **Opaque handle / module encapsulation:** `static` cho mọi thứ nội bộ, header chỉ export API.
- **Lookup table** thay vì tính toán nặng.
- **State machine bằng switch hoặc bảng** (xem `04-application/02-state-machines.md`).
- **Defensive programming:** kiểm tra tham số, trả về `E_NOT_OK`, báo `Det`.

---

## Câu hỏi tự kiểm tra
1. `volatile` có làm biến trở thành atomic không?
2. Tại sao `now - start` vẫn đúng khi timer wrap? Chứng minh với uint8_t.
3. `sizeof` của `struct {uint8_t a; uint16_t b; uint8_t c;}`?
4. Viết macro an toàn để đọc trường bit [11:8] của register.

## Thực hành
- Viết & unit-test (trên PC, dùng Unity) các module: ring buffer, bit utils, endian utils, lookup table nội suy.
