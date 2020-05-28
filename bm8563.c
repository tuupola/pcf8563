/*

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

-cut-

This file is part of hardware agnostic I2C driver for BM8563 RTC:
https://github.com/tuupola/bm8563

SPDX-License-Identifier: MIT

*/

#include <stdint.h>
#include <time.h>

#include "bm8563.h"

static uint8_t decimal2bcd (uint8_t decimal)
{
    return (((decimal / 10) << 4) | (decimal % 10));
}

static uint8_t bcd2decimal(uint8_t bcd)
{
   return (((bcd >> 4) * 10) + (bcd & 0x0f));
}

bm8563_err_t bm8563_init(const bm8563_t *bm)
{
    uint8_t clear = 0x00;
    int32_t status;

    status = bm->write(bm->handle, BM8563_ADDRESS, BM8563_CONTROL_STATUS_1, &clear, 1);
    if (BM8563_OK != status) {
        return status;
    }
    return bm->write(bm->handle, BM8563_ADDRESS, BM8563_CONTROL_STATUS_2, &clear, 1);
}

bm8563_err_t bm8563_read(const bm8563_t *bm, struct tm *time)
{
    uint8_t bcd;
    uint8_t buffer[BM8563_TIME_SIZE] = {0};
    uint16_t century;
    int32_t status;

    status = bm->read(
        bm->handle, BM8563_ADDRESS, BM8563_SECONDS, buffer, BM8563_TIME_SIZE
    );

    if (BM8563_OK != status) {
        return status;
    }

    /* 0..59 */
    bcd = buffer[0] & 0b01111111;
    time->tm_sec = bcd2decimal(bcd);

    /* 0..59 */
    bcd = buffer[1] & 0b01111111;
    time->tm_min = bcd2decimal(bcd);

    /* 0..23 */
    bcd = buffer[2] & 0b00111111;
    time->tm_hour = bcd2decimal(bcd);

    /* 1..31 */
    bcd = buffer[3] & 0b00111111;
    time->tm_mday = bcd2decimal(bcd);

    /* 0..6 */
    bcd = buffer[4] & 0b00000111;
    time->tm_wday = bcd2decimal(bcd);

    /* 0..11 */
    bcd = buffer[5] & 0b00011111;
    time->tm_mon = bcd2decimal(bcd) - 1;

    /* If century bit set assume it is 2000. Note that it seems */
    /* that unlike PCF8563, the BM8563 does NOT automatically */
    /* toggle the century bit when year overflows from 99 to 00. */
    century = (buffer[5] & BM8563_CENTURY_BIT) ? 100 : 0;

    /* Number of years since 1900 */
    bcd = buffer[6] & 0b11111111;
    time->tm_year = bcd2decimal(bcd) + century;

    /* Calculate tm_yday. */
    mktime(time);

    /* low voltage warning */
    if (buffer[0] & 0b10000000) {
        return BM8563_ERR_LOW_VOLTAGE;
    }

    return BM8563_OK;
}

bm8563_err_t bm8563_write(const bm8563_t *bm, const struct tm *time)
{
    uint8_t bcd;
    uint8_t buffer[BM8563_TIME_SIZE] = {0};

    /* 0..59 */
    bcd = decimal2bcd(time->tm_sec);
    buffer[0] = bcd & 0b01111111;

    /* 0..59 */
    bcd = decimal2bcd(time->tm_min);
    buffer[1] = bcd & 0b01111111;

    /* 0..23 */
    bcd = decimal2bcd(time->tm_hour);
    buffer[2] = bcd & 0b00111111;

    /* 1..31 */
    bcd = decimal2bcd(time->tm_mday);
    buffer[3] = bcd & 0b00111111;

    /* 0..6 */
    bcd = decimal2bcd(time->tm_wday);
    buffer[4] = bcd & 0b00000111;

    /* 1..12 */
    bcd = decimal2bcd(time->tm_mon + 1);
    buffer[5] = bcd & 0b00011111;

    /* If 2000 set the century bit. */
    if (time->tm_year >= 100) {
        buffer[5] |= BM8563_CENTURY_BIT;
    }

    /* 0..99 */
    bcd = decimal2bcd(time->tm_year % 100);
    buffer[6] = bcd & 0b11111111;

    return bm->write(bm->handle, BM8563_ADDRESS, BM8563_SECONDS, buffer, BM8563_TIME_SIZE);
}

bm8563_err_t bm8563_close(const bm8563_t *bm)
{
    return BM8563_OK;
}
