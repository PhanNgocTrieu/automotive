# 04 – Diagnostic ở tầng Application

Đọc trước: `01-automotive-fundamentals/03-diagnostics-uds-obd.md`.

## 1. Vai trò
Application (hoặc các monitor trong SWC) **phát hiện lỗi** và báo cho **Dem** (Diagnostic Event Manager). Dem quyết định debounce, lưu DTC, status, freeze frame. Dcm trả lời tester.

```
Monitor (SWC) ── Dem_SetEventStatus(EventId, PASSED/FAILED/PREPASSED/PREFAILED) ──► Dem ──► NvM
                                                                                     ▲
Tester ── 19 02 FF ──► Dcm ──► Dem_GetNextFilteredDTC ───────────────────────────────┘
FiM ◄── Dem (event status) ── chặn chức năng phụ thuộc lỗi
```

## 2. Thiết kế một diagnostic monitor
Mỗi monitor cần định nghĩa:
| Mục | Ví dụ: Headlight open load |
|---|---|
| Điều kiện kích hoạt (enable condition) | Vbat 9–16V, output ON, ignition ON > 500 ms |
| Tiêu chí lỗi | Current sense < 50 mA |
| Debounce | 20 lần × 10 ms liên tiếp = 200 ms |
| Tiêu chí hết lỗi | Current ≥ 100 mA trong 200 ms |
| Phản ứng (reaction) | Báo đèn cảnh báo cluster, gửi CAN status |
| DTC | B1234-13 (open circuit) |
| Freeze frame | Vbat, nhiệt độ, odometer |

## 3. Debounce
### Counter-based (AUTOSAR Dem internal debounce)
```
PREFAILED: counter += stepUp     → counter ≥ FailThreshold  → FAILED
PREPASSED: counter -= stepDown   → counter ≤ PassThreshold  → PASSED
```
```c
typedef struct {
    int16_t cnt;
    int16_t up, down, failTh, passTh;
    uint8_t status;     /* 0 = unknown, 1 = passed, 2 = failed */
} DiagDeb_t;

void DiagDeb_Report(DiagDeb_t *d, bool prefailed)
{
    if (prefailed) {
        d->cnt = (d->cnt + d->up > d->failTh) ? d->failTh : (int16_t)(d->cnt + d->up);
        if (d->cnt >= d->failTh) { d->status = 2u; }
    } else {
        d->cnt = (d->cnt - d->down < d->passTh) ? d->passTh : (int16_t)(d->cnt - d->down);
        if (d->cnt <= d->passTh) { d->status = 1u; }
    }
}
```
### Time-based
Lỗi phải tồn tại liên tục trong T ms.

## 4. Enable conditions & lỗi dây chuyền
- Không kiểm tra khi điều kiện không hợp lệ (ví dụ Vbat thấp lúc crank → mọi output đều "open load" giả).
- **Dependency / suppression:** khi có lỗi gốc (CAN bus-off), không báo hàng loạt lỗi phụ (timeout từng message).
- **Operation cycle:** reset trạng thái "test this cycle" mỗi lần IGN ON.

## 5. Ví dụ đầy đủ: Giám sát điện áp ắc quy
```c
#define VBAT_LOW_mV    (9000u)
#define VBAT_HIGH_mV   (16000u)
#define VBAT_HYST_mV   (300u)

void DiagVbat_Run10ms(uint16_t vbat_mV)
{
    static bool lowActive = false;

    if (vbat_mV < VBAT_LOW_mV)                      { lowActive = true; }
    else if (vbat_mV > (VBAT_LOW_mV + VBAT_HYST_mV)) { lowActive = false; }
    else { }

    Dem_SetEventStatus(DEM_EVENT_VBAT_LOW,
                       lowActive ? DEM_EVENT_STATUS_PREFAILED : DEM_EVENT_STATUS_PREPASSED);

    /* Thông báo trạng thái điện áp cho các monitor khác làm enable condition */
    Rte_Write_VbatInRange((vbat_mV >= VBAT_LOW_mV) && (vbat_mV <= VBAT_HIGH_mV));
}
```

## 6. DID & Routine phía application
- **ReadDataByIdentifier (0x22):** app cung cấp callback đọc giá trị (nhiệt độ, trạng thái, version).
- **IOControl (0x2F):** app phải cho phép override output (test xưởng) và trả quyền khi kết thúc session.
- **RoutineControl (0x31):** ví dụ học lại vị trí cửa sổ (window normalization), calibrate cảm biến.

## 7. Diagnostic specification (CDD/ODX) → code
Thường team diag định nghĩa DTC, DID, routine trong **CANdelaStudio** → generate cấu hình Dcm/Dem; developer viết callback.

---

## Câu hỏi tự kiểm tra
1. Vì sao cần enable condition?
2. Counter-based debounce với up = 2, down = 1, failTh = 10: cần bao nhiêu chu kỳ lỗi liên tiếp để FAILED?
3. FiM dùng để làm gì?
4. Thiết kế monitor phát hiện short-to-ground cho output high-side switch.

## Thực hành
- Viết "mini-Dem": bảng event, debounce counter, DTC status byte theo ISO 14229 (bit 0–3), lưu RAM; hỗ trợ `19 02` và `14` trong UDS server của lab capstone.
