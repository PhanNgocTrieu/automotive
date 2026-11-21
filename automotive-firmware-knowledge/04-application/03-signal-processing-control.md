# 03 – Xử lý tín hiệu & Điều khiển

## 1. Debounce input số

### Counter-based
```c
typedef struct {
    uint8_t cnt;
    uint8_t threshold;      /* số mẫu liên tiếp */
    bool    state;          /* trạng thái đã debounce */
} Debounce_t;

bool Debounce_Update(Debounce_t *d, bool raw)
{
    if (raw != d->state) {
        if (++d->cnt >= d->threshold) {
            d->state = raw;
            d->cnt = 0u;
        }
    } else {
        d->cnt = 0u;
    }
    return d->state;
}
/* Gọi mỗi 5 ms, threshold = 6 → 30 ms */
```

### Phát hiện cạnh, nhấn giữ
```c
bool pressed  = cur && !prev;           /* rising edge */
bool released = !cur && prev;
if (cur) { holdMs += 10u; } else { holdMs = 0u; }
bool longPress = (holdMs >= 800u);
```

## 2. Lọc tín hiệu analog

### Trung bình trượt (Moving average)
```c
#define N 8u
static uint16_t s_buf[N]; static uint8_t s_idx; static uint32_t s_sum;
uint16_t MovAvg(uint16_t x) {
    s_sum = s_sum - s_buf[s_idx] + x;
    s_buf[s_idx] = x;
    s_idx = (uint8_t)((s_idx + 1u) % N);
    return (uint16_t)(s_sum / N);
}
```

### Low-pass IIR bậc 1 (EMA) – fixed point
```
y[n] = y[n-1] + α (x[n] - y[n-1]),     α = Ts / (τ + Ts)
```
```c
/* α = 1/2^k → chỉ cần shift. Lưu y dạng scale 2^k để không mất độ chính xác */
static int32_t s_yScaled;    /* y << K */
#define K 4                  /* α = 1/16 */
int16_t Lpf(int16_t x) {
    s_yScaled += (int32_t)x - (s_yScaled >> K);
    return (int16_t)(s_yScaled >> K);
}
```

### Median filter
Loại bỏ spike (gai nhiễu) – median của 3 hoặc 5 mẫu.

## 3. Scaling & Lookup table
```c
/* Vbat: divider 51k/10k, ADC 12-bit, Vref 3300 mV */
uint16_t vbat_mV = (uint16_t)(((uint32_t)raw * 3300u * 61u) / (4095u * 10u));
```

Lookup table 1D có nội suy:
```c
typedef struct { int16_t x; int16_t y; } Point_t;

int16_t Lut1D(const Point_t *t, uint8_t n, int16_t x)
{
    if (x <= t[0].x)     { return t[0].y; }
    if (x >= t[n-1].x)   { return t[n-1].y; }
    uint8_t i = 1u;
    while (x > t[i].x) { i++; }
    int32_t dx = (int32_t)t[i].x - t[i-1].x;
    int32_t dy = (int32_t)t[i].y - t[i-1].y;
    return (int16_t)(t[i-1].y + (int16_t)(((int32_t)(x - t[i-1].x) * dy) / dx));
}
```
Map 2D (ví dụ bảng phun nhiên liệu theo RPM × tải) – nội suy song tuyến.

## 4. Plausibility & giám sát cảm biến
- **Range check:** raw nằm trong [min, max] hợp lệ (ngoài → short/open).
- **Gradient check:** thay đổi quá nhanh không thực tế (nhiệt độ không thể tăng 50°C trong 10 ms).
- **Cross-check:** 2 kênh dư thừa (APP1 = 2 × APP2 ± tolerance).
- **Stuck check:** giá trị không đổi khi đáng lẽ phải thay đổi.
- Kết quả → **qualified status** + DTC + substitute value.

## 5. Điều khiển PID
```
u(t) = Kp·e + Ki·∫e dt + Kd·de/dt
```
```c
typedef struct {
    float kp, ki, kd;
    float integ, prevErr;
    float outMin, outMax;
    float dt;
} Pid_t;

float Pid_Update(Pid_t *p, float setpoint, float meas)
{
    float err = setpoint - meas;
    float deriv = (err - p->prevErr) / p->dt;
    float out = p->kp * err + p->integ + p->kd * deriv;

    /* anti-windup: chỉ tích phân khi chưa bão hoà (conditional integration) */
    if ((out < p->outMax) && (out > p->outMin)) {
        p->integ += p->ki * err * p->dt;
    }
    if (out > p->outMax) { out = p->outMax; }
    if (out < p->outMin) { out = p->outMin; }
    p->prevErr = err;
    return out;
}
```
Lưu ý: anti-windup, lọc thành phần D (hoặc lấy D trên measurement thay vì error), bumpless transfer, chu kỳ gọi cố định. MCU không có FPU → dùng fixed-point.

Tuning: Ziegler–Nichols, thử nghiệm thủ công (tăng Kp → thêm Ki → thêm Kd), hoặc mô phỏng trên Simulink.

## 6. Ứng dụng điển hình
| Chức năng | Kỹ thuật |
|---|---|
| Quạt làm mát | Lookup table nhiệt độ → duty, hysteresis |
| Điều khiển tốc độ motor DC | PID + PWM + đo tốc độ bằng input capture |
| Chống kẹp cửa sổ | Lọc dòng motor, ngưỡng thích nghi theo vị trí |
| Hiển thị nhiên liệu | Lọc rất chậm (tránh dao động khi xe nghiêng) |
| Đèn LED dimming | Ramp duty (fade in/out) |

---

## Câu hỏi tự kiểm tra
1. Chọn threshold debounce thế nào từ kết quả đo bouncing (lab E3)?
2. IIR với α = 1/16, Ts = 10 ms → hằng số thời gian ≈ ?
3. Integral windup là gì?
4. Thiết kế plausibility check cho cảm biến nhiệt nước làm mát.

## Thực hành
- Unit-test các module: Debounce, MovAvg, Lpf, Lut1D (biên, nội suy, ngoài dải).
- Điều khiển tốc độ motor DC có encoder bằng PID, vẽ đáp ứng bước (log UART → Python matplotlib).
