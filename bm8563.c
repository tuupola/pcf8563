/*

Copyright (c) 2020 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stdint.h>

#include "bm8563.h"

static inline void _i2c_master_write(uint8_t reg, uint8_t data, uint16_t size)
{
    uint8_t buffer[1];
    buffer[0] = data;
    i2c_hal_master_write(BM8563_ADDRESS, reg, buffer, size);
}

static inline void _i2c_master_read(uint8_t reg, uint8_t *buffer, uint16_t size)
{
    i2c_hal_master_read(BM8563_ADDRESS, reg, buffer, size);
}

uint8_t _decimal2bcd (uint8_t decimal)
{
    return (((decimal / 10) << 4) | (decimal % 10));
}

uint8_t _bcd2decimal(uint8_t bcd)
{
   return (((bcd >> 4) * 10) + (bcd & 0x0f));
}

void bm8563_init()
{
    _i2c_master_write(BM8563_CONTROL_STATUS_1, 0x00, 1);
    _i2c_master_write(BM8563_CONTROL_STATUS_2, 0x00, 1);
}

void bm8563_read(bm8563_time_t *time)
{
    uint8_t bcd;
    uint8_t buffer[BM8563_TIME_STRUCT_SIZE];

    _i2c_master_read(BM8563_SECONDS, buffer, BM8563_TIME_STRUCT_SIZE);

    /* TODO: low voltage warning */
    bcd = buffer[0] & 0b01111111;
    time->seconds = _bcd2decimal(bcd);

    bcd = buffer[1] & 0b01111111;
    time->minutes = _bcd2decimal(bcd);

    bcd = buffer[2] & 0b00111111;
    time->hours = _bcd2decimal(bcd);

    bcd = buffer[3] & 0b00111111;
    time->day = _bcd2decimal(bcd);

    bcd = buffer[4] & 0b00000111;
    time->weekday = _bcd2decimal(bcd);

    bcd = buffer[5] & 0b00011111;
    time->month = _bcd2decimal(bcd);

    /* TODO: century */
    bcd = buffer[6] & 0b11111111;
    time->year = _bcd2decimal(bcd);
}

void bm8563_write(bm8563_time_t *time)
{
}

void bm8563_close()
{
}

