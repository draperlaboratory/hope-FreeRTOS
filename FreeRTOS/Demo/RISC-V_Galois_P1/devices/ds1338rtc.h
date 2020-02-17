/**
 * Original code is from: https://github.com/mengguang/DS1338_RTC
 * Modified for Galois P1 purposes
 */
#ifndef __DS1338_H__
#define __DS1338_H__
#include <stdint.h>

// Device address
#define DS1338_ADDR 0x68

// Registers
#define DS1338_REG_SECONDS 0x00
#define DS1338_REG_MINUTES 0x01
#define DS1338_REG_HOURS 0x02
#define DS1338_REG_DAY 0x03
#define DS1338_REG_DATE 0x04
#define DS1338_REG_MONTH 0x05
#define DS1338_REG_YEAR 0x06
#define DS1338_REG_CONTROL 0x07
#define DS1338_REG_RAM_BEGIN 0x08
#define DS1338_REG_RAM_END 0x3F

/**
If set, in an hour register (DS1338_REG_HOURS, DS1338_REG_A1_HOUR,
DS1338_REG_A2_HOUR, the hour is between 0 and 12, and the
(!AM)/PM bit indicates AM or PM.

If not set, the hour is between 0 and 23.
*/
#define DS1338_HOUR_12 (0x01 << 6)

/**
If DS1338_HOUR_12 is set:
- If set, indicates PM
- If not set, indicates AM
*/
#define DS1338_PM_MASK (0x01 << 5)

// If set, the oscillator has stopped since the last time
// this bit was cleared.
#define DS1338_OSC_STOP_FLAG (0x01 << 5)

// Set to disable the oscillator
#define DS1338_OSC_DISABLE (0x01 << 7)

/**
These options control the behavior of the SQW/(!INTB) pin.
*/
#define DS1338_SQWE_FLAG (0x01 << 4)
#define DS1338_SQW_MASK (0x03)
#define DS1338_SQW_32768HZ (0x03)
#define DS1338_SQW_8192HZ (0x02)
#define DS1338_SQW_4096HZ (0x01)
#define DS1338_SQW_1HZ (0x00)

// Occurs when the number of I2C bytes available is less than the number
// requested.
#define READ_ERROR 5

#define decode_bcd(x) ((x >> 4) * 10 + (x & 0x0F))
#define encode_bcd(x) ((((x / 10) & 0x0F) << 4) + (x % 10))

/** Represents a time and date. 
 * Ranges are:
 * Second: 0-59
 * Minute: 0-59
 * Day: 1-7
 * Date: 1-31
 * Month: 1-12
 * Year: 0-99
*/
struct rtctime_t {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
};

void make_time(struct rtctime_t *time, uint8_t year, uint8_t month, uint8_t day,
               uint8_t hour, uint8_t minute, uint8_t second);
void format_time_str(struct rtctime_t *time, char *buf);

uint8_t ds1338_read_time(struct rtctime_t *time);
uint8_t ds1338_write_time(struct rtctime_t *time);

uint8_t ds1338_read_ram(uint8_t address, uint8_t *data, uint8_t length);
uint8_t ds1338_write_ram(uint8_t address, uint8_t *data, uint8_t length);

uint8_t ds1338_enable_sqw(uint8_t frequence);
uint8_t ds1338_disable_sqw(void);

// uint8_t ds1338_get_osf(uint8_t *osf);
uint8_t ds1338_clean_osf(void);

#endif /* __DS1338_H__ */