/**
 * modules.c - Cài đặt các module trong modules.h
 */
#include "modules.h"

/* ======================= C1: Ring buffer ======================= */
void Rb_Init(RingBuf_t *rb)
{
    rb->head = 0u;
    rb->tail = 0u;
}

bool Rb_Put(RingBuf_t *rb, uint8_t b)
{
    bool ok = false;
    uint16_t next = (uint16_t)((rb->head + 1u) & (RB_SIZE - 1u));

    if (next != rb->tail) {           /* chưa đầy (bỏ trống 1 ô để phân biệt full/empty) */
        rb->buf[rb->head] = b;
        rb->head = next;
        ok = true;
    }
    return ok;
}

bool Rb_Get(RingBuf_t *rb, uint8_t *b)
{
    bool ok = false;

    if (rb->tail != rb->head) {
        *b = rb->buf[rb->tail];
        rb->tail = (uint16_t)((rb->tail + 1u) & (RB_SIZE - 1u));
        ok = true;
    }
    return ok;
}

uint16_t Rb_Count(const RingBuf_t *rb)
{
    return (uint16_t)((rb->head - rb->tail) & (RB_SIZE - 1u));
}

/* ======================= C2: Debounce ======================= */
void Debounce_Init(Debounce_t *d, uint8_t threshold, bool initState)
{
    d->cnt = 0u;
    d->threshold = threshold;
    d->state = initState;
}

bool Debounce_Update(Debounce_t *d, bool raw)
{
    if (raw != d->state) {
        d->cnt++;
        if (d->cnt >= d->threshold) {
            d->state = raw;
            d->cnt = 0u;
        }
    } else {
        d->cnt = 0u;
    }
    return d->state;
}

/* ======================= C2: Moving average ======================= */
void MovAvg_Init(MovAvg_t *m, uint16_t initValue)
{
    uint8_t i;
    for (i = 0u; i < MAVG_N; i++) {
        m->buf[i] = initValue;
    }
    m->sum = (uint32_t)initValue * MAVG_N;
    m->idx = 0u;
}

uint16_t MovAvg_Update(MovAvg_t *m, uint16_t x)
{
    m->sum = (m->sum - m->buf[m->idx]) + x;
    m->buf[m->idx] = x;
    m->idx = (uint8_t)((m->idx + 1u) % MAVG_N);
    return (uint16_t)(m->sum / MAVG_N);
}

/* ======================= C2: IIR low-pass (EMA) ======================= */
void Lpf_Init(Lpf_t *f, uint8_t k, int16_t initValue)
{
    f->k = k;
    f->yScaled = (int32_t)initValue * (int32_t)(1L << k);
}

int16_t Lpf_Update(Lpf_t *f, int16_t x)
{
    /* y += (x - y) / 2^k, tính trên giá trị đã scale để giữ phần thập phân.
       Dùng chia (không phải >> số âm) để tránh implementation-defined behavior. */
    int32_t scale = (int32_t)(1L << f->k);
    f->yScaled += (int32_t)x - (f->yScaled / scale);
    return (int16_t)(f->yScaled / scale);
}

/* ======================= C3: Lookup table ======================= */
int16_t Lut1D(const LutPoint_t *table, uint8_t n, int16_t x)
{
    int16_t result;

    if (x <= table[0].x) {
        result = table[0].y;                      /* clamp dưới */
    } else if (x >= table[n - 1u].x) {
        result = table[n - 1u].y;                 /* clamp trên */
    } else {
        uint8_t i = 1u;
        int32_t dx;
        int32_t dy;
        while (x > table[i].x) {
            i++;
        }
        dx = (int32_t)table[i].x - (int32_t)table[i - 1u].x;
        dy = (int32_t)table[i].y - (int32_t)table[i - 1u].y;
        result = (int16_t)((int32_t)table[i - 1u].y +
                           ((((int32_t)x - (int32_t)table[i - 1u].x) * dy) / dx));
    }
    return result;
}

/* ======================= C4: CRC8 SAE J1850 ======================= */
/* poly 0x1D, init 0xFF, xorout 0xFF. Check value "123456789" = 0x4B */
uint8_t Crc8_SaeJ1850(const uint8_t *data, uint32_t len)
{
    uint8_t crc = 0xFFu;
    uint32_t i;
    uint8_t b;

    for (i = 0u; i < len; i++) {
        crc ^= data[i];
        for (b = 0u; b < 8u; b++) {
            if ((crc & 0x80u) != 0u) {
                crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x1Du);
            } else {
                crc = (uint8_t)(crc << 1u);
            }
        }
    }
    return (uint8_t)(crc ^ 0xFFu);
}

/* ======================= C4: CAN signal Intel ======================= */
void Sig_PackIntel(uint8_t buf[8], uint8_t startBit, uint8_t len, uint32_t raw)
{
    uint8_t i;
    for (i = 0u; i < len; i++) {
        uint8_t bit = (uint8_t)(startBit + i);
        uint8_t mask = (uint8_t)(1u << (bit % 8u));
        if (((raw >> i) & 1u) != 0u) {
            buf[bit / 8u] |= mask;
        } else {
            buf[bit / 8u] &= (uint8_t)~mask;
        }
    }
}

uint32_t Sig_UnpackIntel(const uint8_t buf[8], uint8_t startBit, uint8_t len)
{
    uint32_t raw = 0u;
    uint8_t i;
    for (i = 0u; i < len; i++) {
        uint8_t bit = (uint8_t)(startBit + i);
        if (((buf[bit / 8u] >> (bit % 8u)) & 1u) != 0u) {
            raw |= (1UL << i);
        }
    }
    return raw;
}
