/**
 * test_modules.c - Unit test tối giản (không cần framework).
 * Bài tập mở rộng: chuyển sang Unity/Ceedling và đo coverage bằng gcov.
 */
#include <stdio.h>
#include <string.h>
#include "modules.h"

static int g_fail = 0;
static int g_pass = 0;

#define CHECK(cond) do { \
    if (cond) { g_pass++; } \
    else { g_fail++; printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

static void test_ringbuf(void)
{
    RingBuf_t rb;
    uint8_t v = 0u;
    uint8_t i;

    Rb_Init(&rb);
    CHECK(Rb_Get(&rb, &v) == false);              /* rỗng */
    for (i = 0u; i < (RB_SIZE - 1u); i++) {
        CHECK(Rb_Put(&rb, i) == true);
    }
    CHECK(Rb_Put(&rb, 99u) == false);             /* đầy */
    CHECK(Rb_Count(&rb) == (RB_SIZE - 1u));
    CHECK(Rb_Get(&rb, &v) && (v == 0u));          /* FIFO */
    CHECK(Rb_Put(&rb, 100u) == true);             /* wrap-around */
    for (i = 1u; i < (RB_SIZE - 1u); i++) {
        CHECK(Rb_Get(&rb, &v) && (v == i));
    }
    CHECK(Rb_Get(&rb, &v) && (v == 100u));
    CHECK(Rb_Count(&rb) == 0u);
}

static void test_debounce(void)
{
    Debounce_t d;
    Debounce_Init(&d, 3u, false);
    CHECK(Debounce_Update(&d, true) == false);
    CHECK(Debounce_Update(&d, false) == false);   /* bounce → reset counter */
    CHECK(Debounce_Update(&d, true) == false);
    CHECK(Debounce_Update(&d, true) == false);
    CHECK(Debounce_Update(&d, true) == true);     /* 3 mẫu liên tiếp */
    CHECK(Debounce_Update(&d, false) == true);
}

static void test_filters(void)
{
    MovAvg_t m;
    Lpf_t f;
    int i;
    int16_t y = 0;

    MovAvg_Init(&m, 0u);
    for (i = 0; i < (int)MAVG_N; i++) {
        (void)MovAvg_Update(&m, 800u);
    }
    CHECK(MovAvg_Update(&m, 800u) == 800u);

    Lpf_Init(&f, 4u, 0);
    for (i = 0; i < 200; i++) {
        y = Lpf_Update(&f, 1000);
    }
    CHECK((y >= 990) && (y <= 1000));             /* hội tụ về 1000 */

    Lpf_Init(&f, 4u, 0);
    for (i = 0; i < 200; i++) {
        y = Lpf_Update(&f, -1000);
    }
    CHECK((y <= -990) && (y >= -1000));           /* hoạt động với số âm */
}

static void test_lut(void)
{
    /* NTC: nhiệt độ (0.1 °C) theo ADC raw – ví dụ minh hoạ */
    static const LutPoint_t t[] = {
        { 500, 1000 }, { 1500, 500 }, { 2500, 250 }, { 3500, -100 }
    };
    CHECK(Lut1D(t, 4u, 0) == 1000);               /* clamp dưới */
    CHECK(Lut1D(t, 4u, 4000) == -100);            /* clamp trên */
    CHECK(Lut1D(t, 4u, 1500) == 500);             /* đúng điểm */
    CHECK(Lut1D(t, 4u, 1000) == 750);             /* nội suy */
    CHECK(Lut1D(t, 4u, 3000) == 75);
}

static void test_crc8(void)
{
    const uint8_t check[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    CHECK(Crc8_SaeJ1850(check, sizeof(check)) == 0x4Bu);
}

static void test_signal(void)
{
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));

    /* EngineSpeed: start 0, len 16, factor 0.25 → 1000 rpm = raw 4000 = 0x0FA0 */
    Sig_PackIntel(buf, 0u, 16u, 4000u);
    CHECK((buf[0] == 0xA0u) && (buf[1] == 0x0Fu));
    /* CoolantTemp: start 16, len 8, offset -40 → 90 °C = raw 130 */
    Sig_PackIntel(buf, 16u, 8u, 130u);
    CHECK(buf[2] == 130u);
    /* Signal 4 bit không căn byte */
    Sig_PackIntel(buf, 28u, 4u, 0xAu);
    CHECK(Sig_UnpackIntel(buf, 28u, 4u) == 0xAu);
    CHECK(Sig_UnpackIntel(buf, 0u, 16u) == 4000u);
    CHECK(Sig_UnpackIntel(buf, 16u, 8u) == 130u);
}

int main(void)
{
    test_ringbuf();
    test_debounce();
    test_filters();
    test_lut();
    test_crc8();
    test_signal();

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return (g_fail == 0) ? 0 : 1;
}
