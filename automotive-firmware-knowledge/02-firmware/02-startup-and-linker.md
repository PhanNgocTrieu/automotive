# 02 – Startup code, Linker script & Build process

## 1. Quá trình build

```
 .c ──[preprocessor]──► .i ──[compiler]──► .s ──[assembler]──► .o ─┐
                                                                    ├─[linker + .ld]──► .elf ──► .hex/.bin/.s19
 startup.s ─────────────────────────────────────────────────► .o ──┘                    └─► .map
```
- Toolchain: **arm-none-eabi-gcc** (miễn phí), IAR, Keil/ARMCC, GreenHills (MULTI), **TASKING** (TriCore), Wind River Diab, HighTec.
- File output: `.elf` (có debug info), `.hex`/**`.s19` (Motorola S-record)** để flash, `.map` (bố cục bộ nhớ).

```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
  -O2 -g -ffunction-sections -fdata-sections -Wall -Wextra \
  -T linker.ld -Wl,--gc-sections -Wl,-Map=app.map \
  startup.s main.c -o app.elf
arm-none-eabi-size app.elf
arm-none-eabi-objcopy -O ihex app.elf app.hex
arm-none-eabi-objdump -d app.elf > app.lst
```

## 2. Các section bộ nhớ

| Section | Nội dung | Vị trí lưu (LMA) | Vị trí chạy (VMA) |
|---|---|---|---|
| `.isr_vector` | Vector table | Flash | Flash |
| `.text` | Code | Flash | Flash |
| `.rodata` | Hằng số (`const`) | Flash | Flash |
| `.data` | Biến global/static **có** giá trị khởi tạo ≠ 0 | Flash (giá trị ban đầu) | RAM |
| `.bss` | Biến global/static = 0 hoặc không khởi tạo | – | RAM (được xoá về 0) |
| Heap | malloc (thường không dùng trong automotive) | – | RAM |
| Stack | Biến cục bộ, return address | – | RAM |
| `.noinit` | Biến giữ qua reset mềm (ví dụ lưu reset reason) | – | RAM |

```c
int a = 5;              // .data
int b;                  // .bss
static int c = 0;       // .bss
const int d = 10;       // .rodata
void f(void) {
    int e = 1;          // stack
    static int g = 2;   // .data
}
```

## 3. Linker script (GNU ld) – ví dụ tối giản

```ld
ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);
_Min_Stack_Size = 0x1000;

SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH

  .text : {
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
    _etext = .;
  } > FLASH

  _sidata = LOADADDR(.data);
  .data : {
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM AT > FLASH

  .bss (NOLOAD) : {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  .noinit (NOLOAD) : { *(.noinit*) } > RAM

  ._stack_check : {
    . = . + _Min_Stack_Size;
  } > RAM
}
```

## 4. Startup sequence (Reset → main)

```
Power-on / Reset
  └─► CPU đọc SP ban đầu tại địa chỉ 0x0, PC = Reset_Handler tại 0x4
       └─► Reset_Handler:
            1. (Tuỳ chip) tắt watchdog hoặc cấu hình WDG
            2. Khởi tạo RAM ECC (nhiều MCU automotive bắt buộc ghi toàn RAM trước khi đọc)
            3. Copy .data từ Flash → RAM
            4. Xoá .bss về 0
            5. SystemInit(): clock, PLL, flash wait state, FPU enable
            6. Gọi constructor C++ (__libc_init_array) nếu có
            7. main()
```

Startup bằng C (thay vì assembly):
```c
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;
extern int main(void);
void SystemInit(void);

void Reset_Handler(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;

    while (dst < &_edata) { *dst++ = *src++; }
    for (dst = &_sbss; dst < &_ebss; ) { *dst++ = 0u; }

    SystemInit();
    (void)main();
    for (;;) { }            /* main không bao giờ được return */
}

void Default_Handler(void) { for (;;) { } }
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
/* ... */

__attribute__((section(".isr_vector"), used))
void (* const g_vectors[])(void) = {
    (void (*)(void))(&_estack),
    Reset_Handler,
    Default_Handler,        /* NMI */
    HardFault_Handler,
    /* ... */
};
```

## 5. Đọc file .map
- Xem mỗi hàm/biến nằm ở địa chỉ nào, kích thước bao nhiêu.
- Kiểm tra tổng Flash/RAM sử dụng (yêu cầu dự án thường giữ < 70–80% để dự phòng).
- Tìm thư viện lớn bất ngờ (ví dụ printf kéo theo vài chục KB).

## 6. Kỹ thuật linker hay dùng trong automotive
- Đặt hàm vào RAM để chạy khi đang ghi flash: `__attribute__((section(".ramfunc")))`.
- Đặt **calibration data** ở vùng flash riêng (để chỉnh bằng XCP/INCA).
- Tách vùng **bootloader** và **application**, mỗi bên có linker script riêng.
- Vùng **shared RAM** giữa bootloader & app (flag yêu cầu vào programming session).
- Header của application (version, CRC, chữ ký) ở địa chỉ cố định.
- MPU region theo section (ASIL vs QM).

---

## Câu hỏi tự kiểm tra
1. Vì sao `.data` cần cả LMA và VMA?
2. Điều gì xảy ra nếu startup quên xoá `.bss`?
3. Stack overflow có thể gây hậu quả gì? Làm sao phát hiện?
4. Làm sao đặt một hàm chạy từ RAM?

## Thực hành
- Viết project **bare-metal không dùng HAL/IDE**: startup.c + linker.ld + main.c blink LED, build bằng Makefile với arm-none-eabi-gcc. (Mẫu trong `05-labs-projects/code/baremetal_blink/`.)
- Mở file .map, tìm địa chỉ của `main` và tổng kích thước `.bss`.
