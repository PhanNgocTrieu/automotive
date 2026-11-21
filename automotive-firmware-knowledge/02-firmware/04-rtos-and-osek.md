# 04 – RTOS, OSEK/AUTOSAR OS & Scheduling

## 1. Super-loop vs RTOS

**Super-loop (bare-metal):**
```c
int main(void) {
    init();
    for (;;) {
        if (tick10ms) { tick10ms = false; task10ms(); }
        if (tick100ms) { tick100ms = false; task100ms(); }
        background();
    }
}
```
- Đơn giản, đủ cho ECU nhỏ. Nhược điểm: task dài làm trễ task khác, khó quản lý ưu tiên.

**Time-triggered cooperative scheduler:** bảng task + chu kỳ + offset, chạy bởi tick timer. Rất phổ biến trong ECU đơn giản, dễ phân tích.

**RTOS (preemptive):** task ưu tiên cao có thể chen ngang task thấp → đáp ứng real-time tốt hơn.

## 2. Khái niệm RTOS

| Khái niệm | Mô tả |
|---|---|
| Task / Thread | Đơn vị thực thi có stack riêng |
| Trạng thái | Ready, Running, Blocked/Waiting, Suspended |
| Scheduler | Preemptive priority-based (fixed priority) |
| Context switch | Lưu/khôi phục register (PendSV trên Cortex-M) |
| Tick | Ngắt định kỳ (SysTick, 1 ms) |
| Semaphore | Đồng bộ / đếm tài nguyên |
| Mutex | Loại trừ lẫn nhau, có **priority inheritance** |
| Queue / Mailbox | Truyền dữ liệu giữa task |
| Event flags | Chờ nhiều sự kiện |
| Software timer | Callback sau thời gian |

### Vấn đề kinh điển
- **Race condition**: hai luồng truy cập dữ liệu chung không bảo vệ.
- **Deadlock**: A giữ M1 chờ M2, B giữ M2 chờ M1.
- **Priority inversion**: task cao chờ mutex do task thấp giữ, task trung bình chen vào → task cao bị trễ vô hạn (Mars Pathfinder 1997). Giải pháp: **priority inheritance** hoặc **priority ceiling** (OSEK dùng ceiling).
- **Starvation**, **stack overflow**.

## 3. OSEK/VDX OS & AUTOSAR OS

Đặc trưng: **cấu hình tĩnh** (tất cả task, alarm, resource khai báo lúc build trong OIL/ARXML), không tạo task động.

| Khái niệm OSEK | Mô tả |
|---|---|
| **Basic task** | Chạy đến hết rồi terminate, không chờ event (chia sẻ stack được) |
| **Extended task** | Có thể `WaitEvent()` |
| **Alarm** | Gắn với counter, kích hoạt task / set event / callback theo chu kỳ |
| **Resource** | Mutex với **Priority Ceiling Protocol** (`GetResource/ReleaseResource`) |
| **ISR Cat 1** | Không gọi OS API, nhanh nhất |
| **ISR Cat 2** | Được gọi một số OS API (ActivateTask, SetEvent) |
| **Hook** | StartupHook, ErrorHook, PreTaskHook, ShutdownHook |
| Conformance class | BCC1, BCC2, ECC1, ECC2 |

AUTOSAR OS bổ sung:
- **Schedule Table** (thay alarm, time-triggered chính xác, đồng bộ toàn cục).
- **Scalability Class SC1–SC4:** SC2 timing protection, SC3 memory protection (MPU), SC4 cả hai.
- **OS-Application**: nhóm task/ISR có quyền truy cập bộ nhớ riêng (FFI cho ISO 26262).
- **Multi-core**: spinlock, IOC (Inter-OS-Application Communicator).

```c
TASK(Task_10ms)
{
    Rte_Call_10ms_Runnables();   /* hoặc gọi các runnable */
    (void)TerminateTask();
}

ISR(CanRx_ISR)
{
    Can_MainFunction_Read();
    (void)ActivateTask(Task_ComRx);
}
```

## 4. RTOS khác hay gặp
| RTOS | Ghi chú |
|---|---|
| **FreeRTOS** | Miễn phí, phổ biến nhất để học; có SafeRTOS (bản certified) |
| Zephyr | Linux Foundation, đang phát triển mạnh |
| **Vector MICROSAR OS, ETAS RTA-OS, EB tresos AutoCore OS** | AUTOSAR OS thương mại |
| ThreadX (Azure RTOS / Eclipse ThreadX) | Có chứng nhận safety |
| QNX, INTEGRITY, PikeOS | Cho SoC, HPC, hypervisor |
| Erika Enterprise | OSEK open-source |

## 5. Thiết kế task trong ECU

Ví dụ phân bổ:
| Task | Chu kỳ | Priority | Nội dung |
|---|---|---|---|
| Task_1ms | 1 ms | Cao nhất | Điều khiển motor nhanh, ADC trigger |
| Task_5ms | 5 ms | Cao | Can_MainFunction, Com |
| Task_10ms | 10 ms | Trung bình | Application chính, Dem, WdgM |
| Task_100ms | 100 ms | Thấp | Diag, NvM_MainFunction, nhiệt độ |
| Task_Background | idle | Thấp nhất | Flash CRC test, RAM test liên tục |

Nguyên tắc **Rate Monotonic**: chu kỳ ngắn → ưu tiên cao.

## 6. Phân tích thời gian
- **WCET (Worst-Case Execution Time):** đo bằng GPIO toggle + scope, trace (Lauterbach, ARM ETM), hoặc tool tĩnh (aiT).
- **CPU load:** đo thời gian idle. Yêu cầu dự án thường < 70–80%.
- **Schedulability test (RMA):** `U = Σ Ci/Ti ≤ n(2^(1/n) - 1)` (≈ 69% với n lớn) → đủ điều kiện (sufficient).
- **Response time analysis:** `Ri = Ci + Σ_{j∈hp(i)} ⌈Ri/Tj⌉ Cj`.
- Tool: **Timing Architects / Vector TA Tool Suite**, **Gliwa T1**, Lauterbach trace.

## 7. ISR design rules
- ISR ngắn: đọc dữ liệu, clear cờ, đẩy vào queue, kích task → xử lý nặng ở task (**deferred processing**).
- Không gọi hàm blocking, không printf, hạn chế float.
- Ghi chú độ trễ tối đa cho phép (interrupt latency).
- Clear cờ ngắt **trước khi** return (và DSB trên Cortex-M nếu cờ ở peripheral chậm) để tránh vào lại ISR giả.

---

## Câu hỏi tự kiểm tra
1. Basic task và Extended task khác nhau thế nào?
2. Giải thích priority inversion và hai cách chống.
3. Ba task: (C=1,T=4), (C=2,T=6), (C=1,T=10). Có schedulable theo RM không?
4. Vì sao AUTOSAR OS dùng cấu hình tĩnh?
5. ISR Category 1 và 2 khác nhau?

## Thực hành
- Port **FreeRTOS** lên Nucleo: 3 task (LED 500ms, UART log 1s, nút nhấn qua queue).
- Tạo cố ý priority inversion với mutex/semaphore, quan sát bằng GPIO + logic analyzer, rồi bật priority inheritance.
- Viết cooperative scheduler tối giản (mẫu `05-labs-projects/code/scheduler/`).
