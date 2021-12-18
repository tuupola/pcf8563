# Platform agnostic I2C driver for PCF8563 RTC

![C/C++ CI](https://github.com/tuupola/pcf8563/workflows/C/C++%20CI/badge.svg)
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square)](LICENSE)

To use this library you must to provide functions for both reading and writing the I2C bus. Function definitions must be the following.

```c
int32_t i2c_read(void *handle, uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size);
int32_t i2c_write(void *handle, uint8_t address, uint8_t reg, const uint8_t *buffer, uint16_t size);
```

Where `address` is the I2C address, `reg` is the register to read or write, `buffer` holds the data to write or read into and `size` is the amount of data to read or write. The `handle` parameter is an optional customizable argument. You can use it if your I2C implementation requires any additional information such has number of the hardware I2C driver. For example function implementations see [ESP I2C helper](https://github.com/tuupola/esp_i2c_helper). For working example see [M5StickC kitchen sink](https://github.com/tuupola/esp_m5stick).

## Read RTC date and time

```c
#include <time.h>

#include "pcf8563.h"
#include "user_i2c.h"

struct tm rtc;
char buffer[128];
pcf8563_t pcf;

/* Add pointers to user provided functions. */
pcf.read = &user_i2c_read;
pcf.write = &user_i2c_write;

/* You could set the handle here. It can be pointer to anything. */
pcf.handle = NULL;

pcf8563_init(&pcf);
pcf8563_read(&pcf, &rtc);

strftime(buffer, 128 ,"%c (day %j)" , &rtc);
printf("RTC: %s\n", buffer);

```

## Set RTC date and time

```c
#include <time.h>

#include "pcf8563.h"
#include "user_i2c.h"

struct tm rtc;
pcf8563_t pcf;

/* Add pointers to user provided functions. */
pcf.read = &user_i2c_read;
pcf.write = &user_i2c_write;

/* You could set the handle here. It can be pointer to anything. */
pcf.handle = NULL;

/* 2020-12-31 23:59:45 */
rtc.tm_year = 2020 - 1900;
rtc.tm_mon = 12 - 1;
rtc.tm_mday = 31;
rtc.tm_hour = 23;
rtc.tm_min = 59;
rtc.tm_sec = 45;

pcf8563_init(&pcf);
pcf8563_write(&pcf, &rtc);
```

## Set RTC alarm

```c
#include <time.h>

#include "pcf8563.h"
#include "user_i2c.h"

uint8_t tmp;
struct tm rtc_alarm;
pcf8563_t pcf;

/* Add pointers to user provided functions. */
pcf.read = &user_i2c_read;
pcf.write = &user_i2c_write;

pcf8563_init(&pcf);

/* Add alarm every day at 21:30. */
rtc_alarm.tm_min = 30;
rtc_alarm.tm_hour = 21;
rtc_alarm.tm_mday = PCF8563_ALARM_NONE;
rtc_alarm.tm_wday = PCF8563_ALARM_NONE;

pcf8563_ioctl(&pcf, PCF8563_ALARM_SET, &rtc_alarm);

/* Later check if alarm is triggered. */
pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_READ, &tmp);
if (tmp & PCF8563_AF) {
    printf("Got alarm!");
};

/* And clear the alarm flag. */
tmp &= ~PCF8563_AF;
pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_WRITE, &tmp);
```

## Read currently set RTC alarm

```c
#include <time.h>

#include "pcf8563.h"
#include "user_i2c.h"

struct tm rtc_alarm;
pcf8563_t pcf;

/* Add pointers to user provided functions. */
pcf.read = &user_i2c_read;
pcf.write = &user_i2c_write;

pcf8563_init(&pcf);

pcf8563_ioctl(&pcf, PCF8563_ALARM_READ, &rtc_alarm);
```

## Set RTC timer

```c
#include "pcf8563.h"
#include "user_i2c.h"

uint8_t count, control;
pcf8563_t pcf;

/* Add pointers to user provided functions. */
pcf.read = &user_i2c_read;
pcf.write = &user_i2c_write;

pcf8563_init(&pcf);

/* Create a 10 second timer. */
count = 10;
control = PCF8563_TIMER_ENABLE | PCF8563_TIMER_1HZ;

pcf8563_ioctl(&pcf, PCF8563_TIMER_WRITE, &count);
pcf8563_ioctl(&pcf, PCF8563_TIMER_CONTROL_WRITE, &control);

/* Prints "Timer!" every 10 seconds. */
while (1) {
    pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_READ, &control);
    /* Check for timer flag. */
    if (control & PCF8563_TF) {
        printf("Timer!\n");

        /* Clear timer flag. */
        tmp &= ~PCF8563_TF;
        pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_WRITE, &tmp);
    }
}

```


## License

The MIT License (MIT). Please see [License File](LICENSE.txt) for more information.