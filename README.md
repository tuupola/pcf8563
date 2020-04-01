# Hardware agnostic I2C driver for BM8563 RTC

## Read RTC date and time

```c
#include "bm8563.h"
#include "your-i2c-hal.h"

bm8563_datetime_t rtc;

bm8563_init();
bm8563_read(&rtc);

printf(
    "RTC: %04d-%02d-%02d %02d:%02d:%02d\n",
    rtc.year, rtc.month, rtc.day, rtc.hours, rtc.minutes, rtc.seconds
);
```

## Set RTC date and time

```c
#include "bm8563.h"
#include "your-i2c-hal.h"

bm8563_datetime_t rtc;

rtc.year = 2020;
rtc.month = 12;
rtc.day = 31;
rtc.hours = 23;
rtc.minutes = 59;
rtc.seconds = 45;

bm8563_init();
bm8563_write(&rtc);
```