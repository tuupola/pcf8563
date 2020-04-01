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

i2c_read_t i2c_read;
i2c_write_t i2c_write;

uint8_t _decimal2bcd (uint8_t decimal)
{
    return (((decimal / 10) << 4) | (decimal % 10));
}

uint8_t _bcd2decimal(uint8_t bcd)
{
   return (((bcd >> 4) * 10) + (bcd & 0x0f));
}

void bm8563_init(i2c_read_t i2c_read_ptr, i2c_write_t i2c_write_ptr)
{
    uint8_t clear = 0x00;
    i2c_read = i2c_read_ptr;
    i2c_write = i2c_write_ptr;

    i2c_write(BM8563_ADDRESS, BM8563_CONTROL_STATUS_1, &clear, 1);
    i2c_write(BM8563_ADDRESS, BM8563_CONTROL_STATUS_2, &clear, 1);
}

void bm8563_read(bm8563_datetime_t *time)
{
    uint8_t bcd;
    uint8_t buffer[BM8563_TIME_STRUCT_SIZE];
    uint16_t century;

    i2c_read(
        BM8563_ADDRESS, BM8563_SECONDS, buffer, BM8563_TIME_STRUCT_SIZE
    );

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

    /* If century bit set assume it is 2000. Note that it seems */
    /* that unlike PCF8563, the BM8563 does NOT automatically */
    /* toggle the century bit when year overflows from 99 to 00. */
    century = (buffer[5] & BM8563_CENTURY_BIT) ? 2000 : 1900;

    bcd = buffer[6] & 0b11111111;
    time->year = _bcd2decimal(bcd) + century;
}

void bm8563_write(const bm8563_datetime_t *time)
{
    uint8_t bcd;
    uint8_t buffer[BM8563_TIME_STRUCT_SIZE];

    bcd = _decimal2bcd(time->seconds);
    buffer[0] = bcd & 0b01111111;

    bcd = _decimal2bcd(time->minutes);
    buffer[1] = bcd & 0b01111111;

    bcd = _decimal2bcd(time->hours);
    buffer[2] = bcd & 0b00111111;

    bcd = _decimal2bcd(time->day);
    buffer[3] = bcd & 0b00111111;

    bcd = _decimal2bcd(time->weekday);
    buffer[4] = bcd & 0b00000111;

    bcd = _decimal2bcd(time->month);
    buffer[5] = bcd & 0b00011111;

    /* If 2000 set the century bit. */
    if (time->year >= 2000) {
        buffer[5] |= BM8563_CENTURY_BIT;
    }

    bcd = _decimal2bcd(time->year % 100);
    buffer[6] = bcd & 0b11111111;

    i2c_write(BM8563_ADDRESS, BM8563_SECONDS, buffer, BM8563_TIME_STRUCT_SIZE);
}

void bm8563_close()
{
}
