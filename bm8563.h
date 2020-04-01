/*

SPDX-License-Identifier: MIT

MIT License

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

#ifndef _BM8563_H
#define _BM8563_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define	BM8563_ADDRESS	        (0x51)
#define	BM8563_CONTROL_STATUS_1 (0x00)
#define	BM8563_CONTROL_STATUS_2 (0x01)
#define	BM8563_SECONDS          (0x02)
#define	BM8563_MINUTES          (0x03)
#define	BM8563_HOURS            (0x04)
#define	BM8563_DAY              (0x05)
#define	BM8563_WEEKDAY          (0x06)
#define	BM8563_MONTH            (0x07)
#define	BM8563_YEAR             (0x08)
#define	BM8563_TIME_STRUCT_SIZE (0x07)
#define	BM8563_CENTURY_BIT      (0b10000000)

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint16_t year;
} bm8563_datetime_t;

/* These should be provided by the HAL. */
typedef int32_t (*i2c_read_t)(uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size);
typedef int32_t (*i2c_write_t)(uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size);

void bm8563_init(i2c_read_t i2c_read_ptr, i2c_write_t i2c_write_ptr);
void bm8563_read(bm8563_datetime_t *time);
void bm8563_write(const bm8563_datetime_t *time);
void bm8563_close();

#ifdef __cplusplus
}
#endif
#endif
