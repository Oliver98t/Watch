#include "DS3231_rtc.h"
#include "i2c.h"



void rtc_init(void){
    i2c_init();
}

void get_rtc_time(rtc_time* time){
    // 8-bit value to store output of i2c 
    uint8_t register_value;

    // begin i2c write mode for selcting seconds register
    i2c_start(DS3231_WriteMode);
    i2c_byte(DS3231_REG_Seconds);
    i2c_stop();

    i2c_start(DS3231_ReadMode);
    
    register_value = i2c_readAck();
    time->seconds = bcd2dec(register_value);
    
    register_value = i2c_readAck();
    time->minutes = bcd2dec(register_value);
    
    register_value = i2c_readAck();
    time->hours = bcd2dec(register_value);
    
    register_value = i2c_readAck();
    time->weekday = bcd2dec(register_value);
    
    register_value = i2c_readAck();
    time->date = bcd2dec(register_value);
    
    register_value = i2c_readAck();
    time->month = bcd2dec(register_value);
    
    register_value = i2c_readNAck();
    time->year = bcd2dec(register_value) + 2000;

    
    i2c_stop();
}

void set_rtc_time(rtc_time* time){
    uint8_t register_val; // store binary encoded version of time variables

    i2c_start(DS3231_WriteMode);
    i2c_byte(DS3231_REG_Seconds);

    register_val = dec2bcd(time->seconds);
    i2c_byte(register_val);

    register_val = dec2bcd(time->minutes);
    i2c_byte(register_val);

    register_val = dec2bcd(time->hours);
    i2c_byte(register_val);

    i2c_stop();


}

void set_rtc_date(rtc_time* time){
    uint8_t register_val; // store binary encoded version of time variables

    i2c_start(DS3231_WriteMode);
    i2c_byte(DS3231_REG_Date);
    
    register_val = dec2bcd(time->date);
    i2c_byte(register_val);

    register_val = dec2bcd(time->month);
    i2c_byte(register_val);

    // year register can only accept value between 0-99, therefore minus 2000
    register_val = dec2bcd(time->year - 2000);
    i2c_byte(register_val);

    i2c_stop();

}

void set_rtc_weekday(rtc_time* time){
    uint8_t register_val; // store binary encoded version of time variables

    i2c_start(DS3231_WriteMode);
    i2c_byte(DS3231_REG_Day);
    
    register_val = dec2bcd(time->weekday);
    i2c_byte(register_val);

    i2c_stop();

}

uint8_t bcd2dec(uint8_t num) {
	return ((num/16 * 10) + (num % 16));
}

uint8_t dec2bcd(uint8_t num) {
	return ((num/10 * 16) + (num % 10));
}

