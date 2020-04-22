# Hardware agnostic I2C driver for BM8563 RTC

To use this library you must to provide functions for both reading and writing the I2C bus. Function definitions must be the following.

```c
int32_t i2c_read(uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size);
int32_t i2c_write(uint8_t address, uint8_t reg, const uint8_t *buffer, uint16_t size);
```

Where `address` is the I2C address, `reg` is the register to read or write, `buffer` holds the data to write or read into and `size` is the amount of data to read or write. For example  implementation see [ESP I2C helpers](https://github.com/tuupola/esp_i2c_hal). For working example see [M5StickC kitchen sink](https://github.com/tuupola/esp_m5stick).

## Read RTC date and time

```c
#include <time.h>

#include "bm8563.h"
#include "user_i2c.h"

struct tm rtc;
char buffer[128];
bm8563_t bm;

/* Add pointers to user provided functions. */
bm.read = &user_i2c_read;
bm.write = &user_i2c_write;

bm8563_init(&bm);
bm8563_read(&bm, &rtc);

strftime(buffer, 128 ,"%c (day %j)" , &rtc);
printf("RTC: %s\n", buffer);

```

## Set RTC date and time

```c
#include <time.h>

#include "bm8563.h"
#include "user_i2c.h"

struct tm rtc;
bm8563_t bm;

/* Add pointers to user provided functions. */
bm.read = &user_i2c_read;
bm.write = &user_i2c_write;

/* 2020-12-31 23:59:45 */
rtc.tm_year = 2020 - 1900;
rtc.tm_mon = 12 - 1;
rtc.tm_mday = 31;
rtc.tm_hour = 23;
rtc.tm_min = 59;
rtc.tm_sec = 45;

bm8563_init(&bm);
bm8563_write(&bm, &rtc);
```

## License

The MIT License (MIT). Please see [License File](LICENSE.txt) for more information.