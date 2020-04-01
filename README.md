# Hardware agnostic I2C driver for BM8563 RTC

To use this library you must to provide HAL functions for both reading and writing the I2C bus. Function definitions must be the following.

```c
int32_t i2c_read(uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size);
int32_t i2c_write(uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size);
```

Where `address` is the I2C address, `reg` is the register to read or write, `buffer` holds the data to write or read into and `size` is the amount of data to read or write. For example HAL implementation see [ESP I2C master HAL](https://github.com/tuupola/esp_i2c_hal). For minimal working example see [M5Stick RTC demo](https://github.com/tuupola/esp-examples/tree/master/017-m5stick-rtc).

## Read RTC date and time

```c
#include "bm8563.h"
#include "your-i2c-hal.h"

bm8563_datetime_t rtc;

bm8563_init(i2c_read, i2c_write);
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

bm8563_init(i2c_read, i2c_write);
bm8563_write(&rtc);
```

## License

The MIT License (MIT). Please see [License File](LICENSE.txt) for more information.