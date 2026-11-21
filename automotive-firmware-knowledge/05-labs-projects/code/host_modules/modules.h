/**
 * modules.h - Các module tiện ích embedded, viết theo phong cách MISRA-friendly,
 * chạy & test được trên PC (host) trước khi đưa lên MCU.
 *
 * Labs: C1 (ring buffer), C2 (debounce, filter), C3 (lookup table), C4 (CRC8, CAN signal)
 */
#ifndef MODULES_H
#define MODULES_H

#include <stdint.h>
#include <stdbool.h>

/* ---------------- C1: Ring buffer (SPSC, size = lũy thừa 2) ---------------- */
#define RB_SIZE (16u)

typedef struct {
    uint8_t buf[RB_SIZE];
    volatile uint16_t head;   /* producer (ISR) ghi */
    volatile uint16_t tail;   /* consumer (task) ghi */
} RingBuf_t;

void     Rb_Init(RingBuf_t *rb);
bool     Rb_Put(RingBuf_t *rb, uint8_t b);
bool     Rb_Get(RingBuf_t *rb, uint8_t *b);
uint16_t Rb_Count(const RingBuf_t *rb);

/* ---------------- C2: Debounce & filter ---------------- */
typedef struct {
    uint8_t cnt;
    uint8_t threshold;
    bool    state;
} Debounce_t;

void Debounce_Init(Debounce_t *d, uint8_t threshold, bool initState);
bool Debounce_Update(Debounce_t *d, bool raw);

#define MAVG_N (8u)
typedef struct {
    uint16_t buf[MAVG_N];
    uint32_t sum;
    uint8_t  idx;
} MovAvg_t;

void     MovAvg_Init(MovAvg_t *m, uint16_t initValue);
uint16_t MovAvg_Update(MovAvg_t *m, uint16_t x);

typedef struct {
    int32_t yScaled;   /* y << k */
    uint8_t k;         /* alpha = 1 / 2^k */
} Lpf_t;

void    Lpf_Init(Lpf_t *f, uint8_t k, int16_t initValue);
int16_t Lpf_Update(Lpf_t *f, int16_t x);

/* ---------------- C3: Lookup table 1D có nội suy ---------------- */
typedef struct {
    int16_t x;
    int16_t y;
} LutPoint_t;

/* Bảng phải sắp xếp tăng dần theo x, n >= 2 */
int16_t Lut1D(const LutPoint_t *table, uint8_t n, int16_t x);

/* ---------------- C4: CRC & CAN signal ---------------- */
uint8_t  Crc8_SaeJ1850(const uint8_t *data, uint32_t len);

/* Intel (little-endian) signal: start bit = LSB, len 1..32, buffer 8 byte */
void     Sig_PackIntel(uint8_t buf[8], uint8_t startBit, uint8_t len, uint32_t raw);
uint32_t Sig_UnpackIntel(const uint8_t buf[8], uint8_t startBit, uint8_t len);

#endif /* MODULES_H */
