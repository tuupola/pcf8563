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

#ifndef _BM8563_H
#define _BM8563_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define	BM8563_ADDRESS	        (0x34)
#define	BM8563_TIME_STRUCT_SIZE (0x07)

struct bm8563_time
{
    uint8_t second;  /* 0 to 59 */
    uint8_t minute;  /* 0 to 59 */
    uint8_t hour;    /* 0 to 23 */
    uint8_t day;     /* 1 to 31 */
    uint8_t weekday; /* 0 to 6 */
    uint8_t month;   /* 1 to 12 */
    uint16_t year;   /* 0 to 99 */
};

void bm8563_init();
void bm8563_read();
void bm8563_write();
void bm8563_close();

#ifdef __cplusplus
}
#endif
#endif
