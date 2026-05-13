#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "lib/oled-display/lcd.h"
#include "lib/oled-display/i2c.h"
#include "ds3231.h"

// ================== PINS ==================
#define CHG_PIN    PD4
#define CHG_DDR    DDRD
#define CHG_PORT   PORTD
#define CHG_PINREG PIND

#define LED_PIN    PC3
#define LED_DDR    DDRC
#define LED_PORT   PORTC

// ================== LAYOUT ================
// Display: 128x64px, TEXTMODE
// NORMALSIZE: 1 Zeichen = 6px breit, 8px hoch → 21 Zeichen / 8 Zeilen
// DOUBLESIZE: 1 Zeichen = 12px breit, 16px hoch → 10 Zeichen / 4 Zeilen
//
// Uhrzeit (DOUBLESIZE, "HH:MM:SS" = 8 Zeichen = 96px) → x=1 für ~zentriert
// Datum   (NORMALSIZE, "DD.MM.YYYY" = 10 Zeichen = 60px) → x=5 für ~zentriert
// Blitz   (NORMALSIZE, oben rechts) → x=19, y=0

// ================== COMPILE TIME ==========
static ds3231_t compile_time(void) {
    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    const char *date = __DATE__;
    const char *time = __TIME__;

    ds3231_t t;

    char mon_str[4] = {date[0], date[1], date[2], 0};
    t.month = 1;
    for (uint8_t i = 0; i < 12; i++) {
        if (months[i*3]   == mon_str[0] &&
            months[i*3+1] == mon_str[1] &&
            months[i*3+2] == mon_str[2]) {
            t.month = i + 1;
            break;
        }
    }

    t.day  = (date[4] == ' ' ? 0 : date[4] - '0') * 10 + (date[5] - '0');
    t.year = (date[7] - '0') * 1000 + (date[8] - '0') * 100
           + (date[9] - '0') * 10   + (date[10] - '0');

    t.hour = (time[0] - '0') * 10 + (time[1] - '0');
    t.min  = (time[3] - '0') * 10 + (time[4] - '0');
    t.sec  = (time[6] - '0') * 10 + (time[7] - '0');

    return t;
}

// ================== MAIN ==================
int main(void) {
    _delay_ms(1500);

    // LED: Output, aus
    LED_DDR  |=  (1 << LED_PIN);
    LED_PORT &= ~(1 << LED_PIN);

    // CHG: Input mit Pullup (active low)
    CHG_DDR  &= ~(1 << CHG_PIN);
    CHG_PORT |=  (1 << CHG_PIN);

    i2c_init();
    lcd_init(LCD_DISP_ON);

    // Einmalig Zeit setzen — danach auskommentieren und nochmal flashen
    // ds3231_t set = compile_time();
    // ds3231_set(&set);

    char time_str[9]  = {0};  // "HH:MM:SS"
    char date_str[11] = {0};  // "DD.MM.YYYY"
    char prev_time[9]  = {0};
    char prev_date[11] = {0};

    uint8_t prev_charging = 0xFF; // ungültiger Startwert → erster Draw erzwungen
    uint8_t anim_frame    = 0;
    uint8_t anim_tick     = 0;

    while (1) {
        ds3231_t t;
        ds3231_get(&t);

        uint8_t charging = !(CHG_PINREG & (1 << CHG_PIN));

        // LED
        if (charging)
            LED_PORT |=  (1 << LED_PIN);
        else
            LED_PORT &= ~(1 << LED_PIN);

        // Uhrzeit (DOUBLESIZE, zentriert)
        snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", t.hour, t.min, t.sec);
        if (memcmp(time_str, prev_time, sizeof(time_str)) != 0) {
            lcd_charMode(DOUBLESIZE);
            lcd_gotoxy(2, 2);   // y=2 → Zeile 2 (Pixel 16), zentriert vertikal oben
            lcd_puts(time_str);
            lcd_charMode(NORMALSIZE);
            memcpy(prev_time, time_str, sizeof(time_str));
        }

        // Datum (NORMALSIZE, zentriert)
        snprintf(date_str, sizeof(date_str), "%02d.%02d.%04d", t.day, t.month, t.year);
        if (memcmp(date_str, prev_date, sizeof(date_str)) != 0) {
            lcd_charMode(NORMALSIZE);
            lcd_gotoxy(5, 5);   // y=5 → unter der Uhrzeit
            lcd_puts(date_str);
            memcpy(prev_date, date_str, sizeof(date_str));
        }

        // Charging-Anzeige oben rechts
        if (charging != prev_charging || charging) {
            lcd_charMode(NORMALSIZE);

            if (charging) {
                // Blitz-Symbol: "/-\" sieht aus wie ein Blitz in diesem Font
                const char *frames[] = {"/", "/-", "/-\\"};
                anim_tick++;
                if (anim_tick >= 3) {
                    anim_tick = 0;
                    anim_frame = (anim_frame + 1) % 4;
                }
                if (anim_frame == 3) {
                    lcd_gotoxy(18, 0);
                    lcd_puts("   ");   // löschen
                } else {
                    lcd_gotoxy(18, 0);
                    lcd_puts("   ");   // erst löschen
                    lcd_gotoxy(18, 0);
                    lcd_puts(frames[anim_frame]);
                }
            } else {
                // löschen
                lcd_gotoxy(18, 0);
                lcd_puts("   ");
                anim_frame = 0;
                anim_tick  = 0;
            }

            prev_charging = charging;
        }

        _delay_ms(100);
    }
}