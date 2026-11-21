# 08 – Coding Standards: MISRA C & Best Practices

## 1. MISRA C:2012 (và Amendment, MISRA C:2023)
Bộ quy tắc giúp C an toàn, dễ đoán định, dễ phân tích. Phân loại: **Mandatory**, **Required**, **Advisory**. Vi phạm Required phải có **deviation** được phê duyệt.

### Một số rule hay gặp
| Rule | Nội dung | Ví dụ |
|---|---|---|
| Dir 4.6 | Dùng kiểu có kích thước cố định | `uint8_t`, `int32_t` thay vì `int` |
| 10.1–10.8 | Essential type model – không trộn signed/unsigned, không ngầm hạ kiểu | `uint8_t x = a + b;` với a,b uint16 → vi phạm |
| 12.2 | Shift không vượt độ rộng kiểu | |
| 14.4 | Điều kiện if phải là boolean | `if (ptr != NULL)` thay vì `if (ptr)` |
| 15.5 (adv) | Một điểm return | |
| 15.6 | Thân if/for/while luôn có `{}` | |
| 15.7 | `if … else if` phải kết thúc bằng `else` | |
| 16.4 | `switch` phải có `default` | |
| 17.2 | Không đệ quy | |
| 17.7 | Giá trị trả về phải được dùng (hoặc ép `(void)`) | |
| 18.4 | Hạn chế số học con trỏ | |
| 21.3 | Không dùng `malloc/free` | |
| 21.6 | Không dùng stdio (printf) trong production | |
| 8.7 | Biến/hàm chỉ dùng trong 1 file phải `static` | |
| 2.2 | Không có dead code | |

### Ví dụ sửa code
```c
/* ❌ Không tuân thủ */
int calc(int a, int b) {
    if (a) return a * b;
    else return b;
}

/* ✅ Tuân thủ */
static int32_t calc(int32_t a, int32_t b)
{
    int32_t result;

    if (a != 0) {
        result = a * b;     /* cần thêm kiểm tra overflow nếu yêu cầu */
    } else {
        result = b;
    }
    return result;
}
```

## 2. CERT C
Tập trung vào **security**: kiểm tra biên mảng, integer overflow, sử dụng chuỗi an toàn, không undefined behavior.

## 3. Quy ước đặt tên (ví dụ theo AUTOSAR)
```c
Std_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType *PduInfo);
#define CAN_E_PARAM_POINTER   (0x01u)
static uint8 Can_ControllerState[CAN_MAX_CONTROLLERS];
```
- Prefix module: `Adc_`, `Dio_`, `Com_`.
- Macro hằng: chữ hoa, hậu tố `u` cho unsigned (`10u`).
- Kiểu AUTOSAR: `uint8, uint16, uint32, sint8, boolean, Std_ReturnType (E_OK/E_NOT_OK)`.

## 4. Tránh undefined behavior thường gặp
- Signed integer overflow.
- Shift số âm hoặc shift ≥ số bit.
- Đọc biến chưa khởi tạo.
- Truy cập ngoài mảng.
- Vi phạm strict aliasing khi ép kiểu pointer.
- Thứ tự đánh giá: `i = i++ + 1;`

## 5. Static analysis tools
| Tool | Ghi chú |
|---|---|
| **cppcheck** (+ addon misra.py) | Miễn phí, kiểm một phần MISRA |
| clang-tidy | Miễn phí |
| PC-lint Plus | Thương mại, phổ biến |
| Helix QAC | Thương mại, chuẩn MISRA |
| Polyspace Bug Finder / Code Prover | Chứng minh không có runtime error |
| Coverity | Thương mại |

```bash
cppcheck --addon=misra --enable=all --inline-suppr src/
```

## 6. Code review checklist cho firmware
- [ ] Biến chia sẻ ISR ↔ task có `volatile` và được bảo vệ (critical section)?
- [ ] Kiểm tra NULL, kiểm tra biên mảng, kiểm tra độ dài dữ liệu nhận?
- [ ] Có overflow/underflow khi tính toán (nhất là timer, counter, scaling)?
- [ ] Mọi giá trị trả về lỗi đều được xử lý?
- [ ] Không blocking/vòng lặp vô hạn không có timeout?
- [ ] ISR ngắn, không gọi hàm không reentrant?
- [ ] Magic number đã thay bằng macro có tên?
- [ ] Đã chạy static analysis & unit test?
