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
#include <string.h>

#include "pcf8563.h"
#include "mock_i2c.h"

uint8_t memory[255] = {0};

int32_t mock_i2c_read(void *handle, uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size) {
    memcpy(buffer, memory + reg, size);
    return PCF8563_OK;
}

int32_t mock_i2c_write(void *handle, uint8_t address, uint8_t reg, const uint8_t *buffer, uint16_t size) {
    memcpy(memory + reg, buffer, size);
    return PCF8563_OK;
}

int32_t mock_i2c_low_voltage_read(void *handle, uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size) {
    memcpy(buffer, memory + reg, size);
    buffer[0] |= 0b10000000;
    return PCF8563_OK;
}

int32_t mock_failing_i2c_read(void *handle, uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size) {
    return MOCK_I2C_ERROR;
}

int32_t mock_failing_i2c_write(void *handle, uint8_t address, uint8_t reg, const uint8_t *buffer, uint16_t size) {
    return MOCK_I2C_ERROR;
}
