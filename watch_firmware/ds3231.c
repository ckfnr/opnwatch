#include "ds3231.h"
#include "lib/oled-display/i2c.h"

#define DS3231_ADDR 0x68

static uint8_t bcd2dec(uint8_t b) { return (b >> 4) * 10 + (b & 0x0F); }
static uint8_t dec2bcd(uint8_t d) { return ((d / 10) << 4) | (d % 10); }

void ds3231_get(ds3231_t *t) {
    i2c_start(DS3231_ADDR << 1);
    i2c_byte(0x00);
    i2c_start((DS3231_ADDR << 1) | 1);
    t->sec   = bcd2dec(i2c_readAck()  & 0x7F);
    t->min   = bcd2dec(i2c_readAck());
    t->hour  = bcd2dec(i2c_readAck()  & 0x3F);
    i2c_readAck();
    t->day   = bcd2dec(i2c_readAck());
    t->month = bcd2dec(i2c_readAck()  & 0x1F);
    t->year  = bcd2dec(i2c_readNAck()) + 2000;
    i2c_stop();
}

void ds3231_set(const ds3231_t *t) {
    i2c_start(DS3231_ADDR << 1);
    i2c_byte(0x00);
    i2c_byte(dec2bcd(t->sec));
    i2c_byte(dec2bcd(t->min));
    i2c_byte(dec2bcd(t->hour));
    i2c_byte(0x01);                     // Wochentag
    i2c_byte(dec2bcd(t->day));
    i2c_byte(dec2bcd(t->month));
    i2c_byte(dec2bcd(t->year - 2000));
    i2c_stop();
}