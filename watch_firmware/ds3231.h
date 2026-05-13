#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} ds3231_t;

void ds3231_get(ds3231_t *t);
void ds3231_set(const ds3231_t *t);

#endif