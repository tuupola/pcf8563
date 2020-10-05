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

This file is part of hardware agnostic I2C driver for PCF8563 RTC:
https://github.com/tuupola/pcf8563

SPDX-License-Identifier: MIT

*/

#include <stdint.h>
#include <time.h>

#include "pcf8563.h"

static inline uint8_t decimal2bcd (uint8_t decimal)
{
    return (((decimal / 10) << 4) | (decimal % 10));
}

static inline uint8_t bcd2decimal(uint8_t bcd)
{
   return (((bcd >> 4) * 10) + (bcd & 0x0f));
}

pcf8563_err_t pcf8563_init(const pcf8563_t *pcf)
{
    uint8_t clear = 0x00;
    int32_t status;

    status = pcf->write(pcf->handle, PCF8563_ADDRESS, PCF8563_CONTROL_STATUS1, &clear, 1);
    if (PCF8563_OK != status) {
        return status;
    }
    return pcf->write(pcf->handle, PCF8563_ADDRESS, PCF8563_CONTROL_STATUS2, &clear, 1);
}

pcf8563_err_t pcf8563_read(const pcf8563_t *pcf, struct tm *time)
{
    uint8_t bcd;
    uint8_t data[PCF8563_TIME_SIZE] = {0};
    uint16_t century;
    int32_t status;

    status = pcf->read(
        pcf->handle, PCF8563_ADDRESS, PCF8563_SECONDS, data, PCF8563_TIME_SIZE
    );

    if (PCF8563_OK != status) {
        return status;
    }

    /* 0..59 */
    bcd = data[0] & 0b01111111;
    time->tm_sec = bcd2decimal(bcd);

    /* 0..59 */
    bcd = data[1] & 0b01111111;
    time->tm_min = bcd2decimal(bcd);

    /* 0..23 */
    bcd = data[2] & 0b00111111;
    time->tm_hour = bcd2decimal(bcd);

    /* 1..31 */
    bcd = data[3] & 0b00111111;
    time->tm_mday = bcd2decimal(bcd);

    /* 0..6 */
    bcd = data[4] & 0b00000111;
    time->tm_wday = bcd2decimal(bcd);

    /* 0..11 */
    bcd = data[5] & 0b00011111;
    time->tm_mon = bcd2decimal(bcd) - 1;

    /* If century bit set assume it is 2000. */
    century = (data[5] & PCF8563_CENTURY_BIT) ? 100 : 0;

    /* Number of years since 1900. */
    bcd = data[6] & 0b11111111;
    time->tm_year = bcd2decimal(bcd) + century;

    /* Calculate tm_yday. */
    mktime(time);

    /* low voltage warning */
    if (data[0] & 0b10000000) {
        return PCF8563_ERR_LOW_VOLTAGE;
    }

    return PCF8563_OK;
}

pcf8563_err_t pcf8563_write(const pcf8563_t *pcf, const struct tm *time)
{
    uint8_t bcd;
    uint8_t data[PCF8563_TIME_SIZE] = {0};

    /* 0..59 */
    bcd = decimal2bcd(time->tm_sec);
    data[0] = bcd & 0b01111111;

    /* 0..59 */
    bcd = decimal2bcd(time->tm_min);
    data[1] = bcd & 0b01111111;

    /* 0..23 */
    bcd = decimal2bcd(time->tm_hour);
    data[2] = bcd & 0b00111111;

    /* 1..31 */
    bcd = decimal2bcd(time->tm_mday);
    data[3] = bcd & 0b00111111;

    /* 0..6 */
    bcd = decimal2bcd(time->tm_wday);
    data[4] = bcd & 0b00000111;

    /* 1..12 */
    bcd = decimal2bcd(time->tm_mon + 1);
    data[5] = bcd & 0b00011111;

    /* If 2000 set the century bit. */
    if (time->tm_year >= 100) {
        data[5] |= PCF8563_CENTURY_BIT;
    }

    /* 0..99 */
    bcd = decimal2bcd(time->tm_year % 100);
    data[6] = bcd & 0b11111111;

    return pcf->write(pcf->handle, PCF8563_ADDRESS, PCF8563_SECONDS, data, PCF8563_TIME_SIZE);
}

pcf8563_err_t pcf8563_ioctl(const pcf8563_t *pcf, int16_t command, void *buffer)
{
    uint8_t reg = command >> 8;
    uint8_t data[PCF8563_ALARM_SIZE] = {0};
    uint8_t status;
    struct tm *time;

    switch (command) {
    case PCF8563_ALARM_SET:
        time = (struct tm *)buffer;

        /* 0..59 */
        if (PCF8563_ALARM_NONE == time->tm_min) {
            data[0] = PCF8563_ALARM_DISABLE;
        } else {
            data[0] = decimal2bcd(time->tm_min);
        }

        /* 0..23 */
        if (PCF8563_ALARM_NONE == time->tm_hour) {
            data[1] = PCF8563_ALARM_DISABLE;
        } else {
            data[1] = decimal2bcd(time->tm_hour);
            data[1] &= 0b00111111;
        }

        /* 1..31 */
        if (PCF8563_ALARM_NONE == time->tm_mday) {
            data[2] = PCF8563_ALARM_DISABLE;
        } else {
            data[2] = decimal2bcd(time->tm_mday);
            data[2] &= 0b00111111;
        }

        /* 0..6 */
        if (PCF8563_ALARM_NONE == time->tm_mday) {
            data[3] = PCF8563_ALARM_DISABLE;
        } else {
            data[3] = decimal2bcd(time->tm_wday);
            data[3] &= 0b00000111;
        }

        return pcf->write(
            pcf->handle, PCF8563_ADDRESS, reg, data, PCF8563_ALARM_SIZE
        );

        break;

    case PCF8563_ALARM_READ:
        time = (struct tm *)buffer;

        /* 0..59 */
        status = pcf->read(
            pcf->handle, PCF8563_ADDRESS, reg, data, PCF8563_ALARM_SIZE
        );

        if (PCF8563_OK != status) {
            return status;
        }

        if (PCF8563_ALARM_DISABLE & data[0]) {
            time->tm_min = PCF8563_ALARM_NONE;
        } else {
            data[0] &= 0b01111111;
            time->tm_min = bcd2decimal(data[0]);
        }

        /* 0..23 */
        if (PCF8563_ALARM_DISABLE & data[1]) {
            time->tm_hour = PCF8563_ALARM_NONE;
        } else {
            data[1] &= 0b00111111;
            time->tm_hour = bcd2decimal(data[1]);
        }

        /* 1..31 */
        if (PCF8563_ALARM_DISABLE & data[2]) {
            time->tm_mday = PCF8563_ALARM_NONE;
        } else {
            data[2] &= 0b00111111;
            time->tm_mday = bcd2decimal(data[2]);
        }

        /* 0..6 */
        if (PCF8563_ALARM_DISABLE & data[3]) {
            time->tm_wday = PCF8563_ALARM_NONE;
        } else {
            data[3] &= 0b00000111;
            time->tm_wday = bcd2decimal(data[3]);
        }

        return PCF8563_OK;
        break;

    case PCF8563_CONTROL_STATUS1_READ:
    case PCF8563_CONTROL_STATUS2_READ:
    case PCF8563_TIMER_CONTROL_READ:
    case PCF8563_TIMER_READ:
        return pcf->read(
            pcf->handle, PCF8563_ADDRESS, reg, (uint8_t *)buffer, 1
        );
        break;

    case PCF8563_CONTROL_STATUS1_WRITE:
    case PCF8563_CONTROL_STATUS2_WRITE:
    case PCF8563_TIMER_CONTROL_WRITE:
    case PCF8563_TIMER_WRITE:
        return pcf->write(
            pcf->handle, PCF8563_ADDRESS, reg, (uint8_t *)buffer, 1
        );
        break;

    }

    return PCF8563_ERROR_NOTTY;
}

pcf8563_err_t pcf8563_close(const pcf8563_t *pcf)
{
    return PCF8563_OK;
}
