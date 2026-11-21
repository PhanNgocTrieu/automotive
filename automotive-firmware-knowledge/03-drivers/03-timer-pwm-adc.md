# 03 – Timer, PWM, Input Capture, ADC, DMA

## 1. Timer cơ bản
```
f_timer = f_clk / (PSC + 1)
T_overflow = (ARR + 1) / f_timer
```
Ví dụ: f_clk = 84 MHz, PSC = 83 → 1 MHz (1 µs/tick); ARR = 999 → ngắt mỗi 1 ms.

Chế độ: up/down/center-aligned count, one-pulse, output compare, input capture, PWM, encoder mode.

**AUTOSAR GPT** (General Purpose Timer): `Gpt_StartTimer`, `Gpt_EnableNotification`, `Gpt_GetTimeElapsed`.

Timer đặc thù automotive: **GTM** (Generic Timer Module) trên AURIX/SPC5 — cực mạnh, có sub-module TIM/TOM/ATOM/MCS (lập trình được) cho điều khiển động cơ; **eMIOS** (NXP); **FlexTimer FTM** (S32K); **TAUD/TAUJ** (RH850).

## 2. PWM
```
Edge-aligned:  duty = CCR / (ARR + 1)
Center-aligned: dùng cho motor control, giảm nhiễu hài
```
```c
void Pwm_SetDuty(uint16_t duty_permille)   /* 0..1000 */
{
    if (duty_permille > 1000u) { duty_permille = 1000u; }
    TIM3->CCR1 = (uint32_t)(((uint32_t)TIM3->ARR + 1u) * duty_permille / 1000u);
}
```
- Cập nhật duty có **preload/shadow register** → thay đổi đồng bộ ở đầu chu kỳ, tránh glitch.
- Motor 3 pha: complementary output + **dead-time** để tránh shoot-through.
- AUTOSAR PWM: `Pwm_SetDutyCycle(ch, 0x0000..0x8000)` (0x8000 = 100%), `Pwm_SetPeriodAndDuty`, `Pwm_SetOutputToIdle`.

## 3. Input Capture
Đo tần số / duty của tín hiệu vào (cảm biến tốc độ, PWM từ cảm biến):
```
period = capture[n] - capture[n-1]        (unsigned → tự xử lý wrap)
freq   = f_timer / period
duty   = (falling - rising) / period
```
AUTOSAR **ICU**: `Icu_EnableEdgeDetection`, `Icu_StartSignalMeasurement`, `Icu_GetDutyCycleValues`, `Icu_GetTimeElapsed`.
Chú ý tín hiệu rất chậm hoặc mất tín hiệu → timeout → speed = 0.

## 4. ADC

### Cấu hình
- Kênh (channel), **group/sequence**, độ phân giải, sample time.
- Trigger: software, **hardware (timer)** – đồng bộ với PWM để đo dòng đúng thời điểm.
- Chế độ: single, continuous, scan, injected (ưu tiên cao).
- Kết quả: polling cờ EOC, interrupt, **DMA**.

### Driver ADC đơn giản (polling)
```c
uint16_t Adc_ReadChannel(uint8_t ch)
{
    uint32_t timeout = ADC_TIMEOUT;
    ADC1->SQR3 = ch;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (((ADC1->SR & ADC_SR_EOC) == 0u) && (timeout > 0u)) { timeout--; }
    if (timeout == 0u) { return ADC_INVALID; }        /* báo lỗi */
    return (uint16_t)ADC1->DR;                         /* đọc DR clear EOC */
}
```

### AUTOSAR ADC
```c
Adc_SetupResultBuffer(AdcGroup_Sensors, s_adcBuf);
Adc_StartGroupConversion(AdcGroup_Sensors);
/* ... trong notification hoặc polling */
if (Adc_GetGroupStatus(AdcGroup_Sensors) == ADC_STREAM_COMPLETED) {
    Adc_ReadGroup(AdcGroup_Sensors, results);
}
```

### Xử lý sau ADC
- Lọc (trung bình trượt, IIR) – xem `04-application/03-signal-processing-control.md`.
- Scaling raw → đơn vị vật lý.
- **Kiểm tra dải hợp lệ**: raw gần 0 → short to GND; gần max → open/short to battery → DTC.

## 5. DMA
Chuyển dữ liệu giữa peripheral ↔ RAM không cần CPU.
- Cấu hình: source, destination, kích thước, số lượng, increment, circular mode, ngắt half/full transfer.
- Ứng dụng: ADC scan liên tục, UART RX buffer, SPI truyền khối.
- Lưu ý: cache coherency (M7), buffer phải aligned, không đọc buffer khi DMA đang ghi (dùng ping-pong / half-transfer interrupt).

---

## Câu hỏi tự kiểm tra
1. Tính PSC/ARR để tạo PWM 20 kHz với clock 80 MHz, độ phân giải duty tối đa.
2. Shadow register giúp gì khi cập nhật duty?
3. Dead-time là gì?
4. Làm sao phát hiện cảm biến analog bị đứt dây qua ADC?
5. Khi nào cần trigger ADC bằng timer?

## Thực hành
- Timer 1 ms → tick hệ thống (thay SysTick).
- PWM điều chỉnh độ sáng LED theo biến trở (ADC → duty).
- Input capture đo tần số tín hiệu từ function generator / PWM của chính MCU.
- ADC scan 4 kênh bằng DMA circular, tính trung bình 8 mẫu.
