#ifndef DS3231_RTC_H
#define DS3231_RTC_H

#include <stdio.h>

#define DS3231_ReadMode             (0xD1)
#define DS3231_WriteMode            (0xD0)
#define DS3231_REG_Seconds          (0x00)
#define DS3231_REG_Minutes          (0x01)
#define DS3231_REG_Hour             (0x02)
#define DS3231_REG_Day              (0x03)
#define DS3231_REG_Date             (0x04)
#define DS3231_REG_Month            (0x05)
#define DS3231_REG_Year             (0x06)
#define DS3231_REG_ALARM_1          (0x07)
#define DS3231_REG_ALARM_2          (0x0B)
#define DS3231_REG_CONTROL          (0x0E)
#define DS3231_REG_STATUS           (0x0F)
#define DS3231_REG_TEMPERATURE      (0x11)

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t weekday;
    uint8_t date;
    uint8_t month;
    uint16_t year;
}rtc_time;



void rtc_init(void);
void get_rtc_time(rtc_time* time);
void set_rtc_time(rtc_time* time);
void set_rtc_date(rtc_time* time);
void set_rtc_weekday(rtc_time* time);
uint8_t bcd2dec(uint8_t num);
uint8_t dec2bcd(uint8_t num);



#endif