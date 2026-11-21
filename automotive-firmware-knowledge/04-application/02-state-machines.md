# 02 – State Machine & Mode Management

State machine là công cụ thiết kế quan trọng nhất ở tầng application automotive: cửa sổ điện, gạt mưa, khoá cửa, quản lý chế độ ECU, sạc EV…

## 1. Thành phần
- **State:** trạng thái.
- **Event / Condition:** điều kiện chuyển.
- **Transition:** chuyển trạng thái.
- **Action:** entry / exit / during (do) / on transition.

## 2. Ví dụ: Cửa sổ điện (Power Window) với chống kẹp (anti-pinch)

```
                 btnUp (short)            reachTop
   ┌────────┐ ────────────────► ┌──────────────┐ ──────► ┌────────┐
   │  IDLE  │                   │  MOVING_UP   │         │ CLOSED │
   └────────┘ ◄──────────────── └──────────────┘         └────────┘
     ▲    │     btnRelease          │ pinchDetected (current > limit)
     │    │ btnDown                 ▼
     │    ▼                   ┌──────────────┐
     │ ┌──────────────┐       │   REVERSING  │ (chạy xuống 200 ms / 10 cm)
     └─┤ MOVING_DOWN  │       └──────────────┘
       └──────────────┘              │ timeout
                                     ▼
                                   IDLE
   Bất kỳ state nào: motorOverTemp → BLOCKED (chờ nguội)
```

## 3. Cài đặt bằng switch-case
```c
typedef enum { WIN_IDLE, WIN_UP, WIN_DOWN, WIN_REVERSE, WIN_BLOCKED } WinState_t;

typedef struct {
    bool btnUp, btnDown, atTop, atBottom, pinch, overTemp;
} WinInput_t;

static WinState_t s_state = WIN_IDLE;
static uint16_t   s_timer_ms = 0u;

static void enter(WinState_t st)
{
    s_state = st;
    s_timer_ms = 0u;
    switch (st) {
        case WIN_UP:      Motor_Set(MOTOR_UP);   break;
        case WIN_DOWN:
        case WIN_REVERSE: Motor_Set(MOTOR_DOWN); break;
        case WIN_IDLE:
        case WIN_BLOCKED:
        default:          Motor_Set(MOTOR_STOP); break;
    }
}

void Window_Run10ms(const WinInput_t *in)
{
    s_timer_ms += 10u;

    if (in->overTemp && (s_state != WIN_BLOCKED)) { enter(WIN_BLOCKED); return; }

    switch (s_state) {
        case WIN_IDLE:
            if (in->btnUp && !in->atTop)            { enter(WIN_UP); }
            else if (in->btnDown && !in->atBottom)  { enter(WIN_DOWN); }
            else { }
            break;
        case WIN_UP:
            if (in->pinch)                          { enter(WIN_REVERSE); }
            else if (in->atTop || !in->btnUp)       { enter(WIN_IDLE); }
            else { }
            break;
        case WIN_DOWN:
            if (in->atBottom || !in->btnDown)       { enter(WIN_IDLE); }
            break;
        case WIN_REVERSE:
            if ((s_timer_ms >= 300u) || in->atBottom) { enter(WIN_IDLE); }
            break;
        case WIN_BLOCKED:
            if (!in->overTemp && (s_timer_ms >= 5000u)) { enter(WIN_IDLE); }
            break;
        default:
            enter(WIN_IDLE);        /* phòng thủ: state không hợp lệ */
            break;
    }
}
```

## 4. Cài đặt bằng bảng (table-driven)
```c
typedef struct {
    WinState_t from;
    bool (*guard)(const WinInput_t *in);
    WinState_t to;
} Transition_t;

static const Transition_t s_table[] = {
    { WIN_IDLE, g_upPressed,   WIN_UP   },
    { WIN_IDLE, g_downPressed, WIN_DOWN },
    { WIN_UP,   g_pinch,       WIN_REVERSE },
    { WIN_UP,   g_upDone,      WIN_IDLE },
    /* ... */
};
```
Ưu điểm: dễ review, dễ sinh từ tool; nhược: khó debug khi logic phức tạp.

## 5. Hierarchical State Machine (HSM)
Trạng thái lồng nhau (ví dụ `OPERATIONAL { NORMAL, DEGRADED }`, `FAULT`). Có thể dùng framework **QP/C (Quantum Leaps)**, hoặc thiết kế bằng **Stateflow** (Simulink) rồi generate code.

## 6. ECU Mode Management
```
 POWER_OFF ─► STARTUP ─► RUN ◄─► POST_RUN ─► SHUTDOWN / SLEEP
                          │
                          └─► (lỗi nghiêm trọng) ─► SAFE_STATE / LIMP_HOME
```
- **STARTUP:** init driver, NvM_ReadAll, self-test.
- **RUN:** chức năng đầy đủ khi KL15 ON hoặc có yêu cầu mạng.
- **POST_RUN:** KL15 OFF, hoàn tất (quạt làm mát tiếp, ghi NvM).
- **SLEEP:** chờ wake-up.
AUTOSAR: **EcuM** (startup/shutdown), **BswM** (rule-based mode arbitration), **ComM**.

## 7. Checklist review state machine
- [ ] Mọi state đều có cách thoát (không kẹt)?
- [ ] Điều kiện chuyển có loại trừ nhau/ưu tiên rõ ràng?
- [ ] Có xử lý state không hợp lệ (default)?
- [ ] Event xảy ra đồng thời được xử lý thế nào?
- [ ] Timeout cho các state chờ?
- [ ] Action khi vào/ra state được thực hiện đúng một lần?

---

## Thực hành
- Cài đặt state machine cửa sổ điện, viết unit test cho mọi transition (bao gồm pinch).
- Mô phỏng trên PC: input từ bàn phím, output in ra console.
- Thiết kế state machine gạt mưa: OFF / INT (theo núm chỉnh) / LOW / HIGH / AUTO (cảm biến mưa) / MIST (1 lần quét), có yêu cầu "park position" khi tắt.
