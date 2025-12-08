/*

MIT License

Copyright (c) 2020-2021 Mika Tuupola

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

#include "greatest.h"
#include "pcf8563.h"
#include "mock_i2c.h"

TEST
should_pass(void)
{
    PASS();
}

TEST
should_fail_init(void)
{
    pcf8563_t bm;
    uint32_t status;
    bm.read = &mock_failing_i2c_read;
    bm.write = &mock_failing_i2c_write;

    ASSERT_FALSE(PCF8563_OK == pcf8563_init(&bm));
    PASS();
}

TEST
should_init(void)
{
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    PASS();
}

TEST
should_fail_read_time(void)
{
    struct tm datetime = {0};
    pcf8563_t bm;
    bm.read = &mock_failing_i2c_read;
    bm.write = &mock_i2c_write;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT_FALSE(PCF8563_OK == pcf8563_read(&bm, &datetime));
    PASS();
}

TEST
should_get_low_voltage_warning(void)
{
    struct tm datetime = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_low_voltage_read;
    bm.write = &mock_i2c_write;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_ERR_LOW_VOLTAGE == pcf8563_read(&bm, &datetime));
    PASS();
}

TEST
should_read_and_write_time(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    char buffer[128];
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    datetime.tm_sec = 35;
    datetime.tm_min = 15;
    datetime.tm_hour = 23;
    datetime.tm_mday = 27;
    /* Months since January. */
    datetime.tm_mon = 11 - 1;
    /* Years since 1900. */
    datetime.tm_year = 2002 - 1900;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_write(&bm, &datetime));
    ASSERT(PCF8563_OK == pcf8563_read(&bm, &datetime2));

    ASSERT_EQ(datetime.tm_sec, datetime2.tm_sec);
    ASSERT_EQ(datetime.tm_min, datetime2.tm_min);
    ASSERT_EQ(datetime.tm_hour, datetime2.tm_hour);
    ASSERT_EQ(datetime.tm_mon, datetime2.tm_mon);
    ASSERT_EQ(datetime.tm_year, datetime2.tm_year);

    PASS();
}

TEST
should_handle_year_1900(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    datetime.tm_sec = 0;
    datetime.tm_min = 0;
    datetime.tm_hour = 0;
    datetime.tm_mday = 1;
    /* Months since January. */
    datetime.tm_mon = 0;
    /* Years since 1900, no century bit. */
    datetime.tm_year = 1900 - 1900;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_write(&bm, &datetime));
    ASSERT(PCF8563_OK == pcf8563_read(&bm, &datetime2));

    ASSERT_EQ(datetime.tm_year, datetime2.tm_year);
    PASS();
}

TEST
should_handle_year_1999(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    datetime.tm_sec = 59;
    datetime.tm_min = 59;
    datetime.tm_hour = 23;
    datetime.tm_mday = 31;
    /* Months since January. */
    datetime.tm_mon = 11;
    /* Years since 1900, no century bit. */
    datetime.tm_year = 1999 - 1900;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_write(&bm, &datetime));
    ASSERT(PCF8563_OK == pcf8563_read(&bm, &datetime2));

    ASSERT_EQ(datetime.tm_year, datetime2.tm_year);
    PASS();
}

TEST
should_handle_year_2000(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    datetime.tm_sec = 0;
    datetime.tm_min = 0;
    datetime.tm_hour = 0;
    datetime.tm_mday = 1;
    /* Months since January. */
    datetime.tm_mon = 0;
    /* Years since 1900, century bit should be set. */
    datetime.tm_year = 2000 - 1900;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_write(&bm, &datetime));
    ASSERT(PCF8563_OK == pcf8563_read(&bm, &datetime2));

    ASSERT_EQ(datetime.tm_year, datetime2.tm_year);
    PASS();
}

TEST
should_handle_year_2099(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    datetime.tm_sec = 59;
    datetime.tm_min = 59;
    datetime.tm_hour = 23;
    datetime.tm_mday = 31;
    datetime.tm_mon = 11;
    /* Years since 1900, century bit should be set. */
    datetime.tm_year = 2099 - 1900;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_write(&bm, &datetime));
    ASSERT(PCF8563_OK == pcf8563_read(&bm, &datetime2));

    ASSERT_EQ(datetime.tm_year, datetime2.tm_year);
    PASS();
}

TEST
should_read_and_write_alarm(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    datetime.tm_min = 30;
    datetime.tm_hour = 21;
    datetime.tm_mday = 15;
    datetime.tm_wday = 3;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_ALARM_SET, &datetime));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_ALARM_READ, &datetime2));
    ASSERT_EQ(datetime.tm_min, datetime2.tm_min);
    ASSERT_EQ(datetime.tm_hour, datetime2.tm_hour);
    ASSERT_EQ(datetime.tm_mday, datetime2.tm_mday);
    ASSERT_EQ(datetime.tm_wday, datetime2.tm_wday);

    PASS();
}

TEST
should_read_and_write_alarm_all_none(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    datetime.tm_min = PCF8563_ALARM_NONE;
    datetime.tm_hour = PCF8563_ALARM_NONE;
    datetime.tm_mday = PCF8563_ALARM_NONE;
    datetime.tm_wday = PCF8563_ALARM_NONE;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_ALARM_SET, &datetime));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_ALARM_READ, &datetime2));
    ASSERT_EQ(datetime.tm_min, datetime2.tm_min);
    ASSERT_EQ(datetime.tm_hour, datetime2.tm_hour);
    ASSERT_EQ(datetime.tm_mday, datetime2.tm_mday);
    ASSERT_EQ(datetime.tm_wday, datetime2.tm_wday);

    PASS();
}

TEST
should_read_and_write_timer(void)
{
    uint8_t count = 10;
    uint8_t reg =  PCF8563_TIMER_ENABLE | PCF8563_TIMER_1HZ;

    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_TIMER_WRITE, &count));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_TIMER_CONTROL_WRITE, &reg));

    count = 0;
    reg = 0;

    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_TIMER_READ, &count));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_TIMER_CONTROL_READ, &reg));
    ASSERT(10 == count);
    ASSERT((PCF8563_TIMER_ENABLE | PCF8563_TIMER_1HZ) == reg);

    PASS();
}

TEST
should_return_error_for_invalid_ioctl(void)
{
    uint8_t dummy;
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_ERROR_NOTTY == pcf8563_ioctl(&bm, 0x9999, &dummy));

    PASS();
}

TEST
should_fail_alarm_read(void)
{
    struct tm datetime = {0};
    pcf8563_t bm;
    bm.read = &mock_failing_i2c_read;
    bm.write = &mock_i2c_write;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT_FALSE(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_ALARM_READ, &datetime));

    PASS();
}

TEST
should_close(void)
{
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_close(&bm));

    PASS();
}

TEST
bug_4_should_set_alarm_weekday(void)
{
    struct tm datetime = {0};
    struct tm datetime2 = {0};
    pcf8563_t bm;
    bm.read = &mock_i2c_read;
    bm.write = &mock_i2c_write;

    /* Set weekday alarm enabled but mday alarm disabled. */
    /* This tests that weekday uses tm_wday, not tm_mday. */
    datetime.tm_min = PCF8563_ALARM_NONE;
    datetime.tm_hour = PCF8563_ALARM_NONE;
    datetime.tm_mday = PCF8563_ALARM_NONE;
    datetime.tm_wday = 3;

    ASSERT(PCF8563_OK == pcf8563_init(&bm));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_ALARM_SET, &datetime));
    ASSERT(PCF8563_OK == pcf8563_ioctl(&bm, PCF8563_ALARM_READ, &datetime2));

    /* Weekday should be 3, not disabled. */
    ASSERT_EQ(datetime.tm_wday, datetime2.tm_wday);
    /* mday should be disabled (ALARM_NONE). */
    ASSERT_EQ(PCF8563_ALARM_NONE, datetime2.tm_mday);

    PASS();
}

GREATEST_MAIN_DEFS();

int
main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_TEST(should_pass);
    RUN_TEST(should_fail_init);
    RUN_TEST(should_init);
    RUN_TEST(should_fail_read_time);
    RUN_TEST(should_get_low_voltage_warning);
    RUN_TEST(should_read_and_write_time);
    RUN_TEST(should_handle_year_1900);
    RUN_TEST(should_handle_year_1999);
    RUN_TEST(should_handle_year_2000);
    RUN_TEST(should_handle_year_2099);
    RUN_TEST(should_read_and_write_alarm);
    RUN_TEST(should_read_and_write_alarm_all_none);
    RUN_TEST(should_read_and_write_timer);
    RUN_TEST(should_return_error_for_invalid_ioctl);
    RUN_TEST(should_fail_alarm_read);
    RUN_TEST(should_close);

    RUN_TEST(bug_4_should_set_alarm_weekday);

    GREATEST_MAIN_END();
}
