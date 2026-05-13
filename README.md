# opnwatch

**opnwatch** is an open-source wristwatch based on the ATmega328P microcontroller.

## Hardware

| Component | Part |
|-----------|------|
| Microcontroller | ATmega328P |
| Display | SSD1306 OLED |
| RTC | DS3231M |
| Charging IC | BQ24075RGT |
| PCB | Custom, designed in KiCad |

## Repository Structure

```
opnwatch/
├── CAD_files/        # KiCad schematic and PCB layout
└── watch_firmware/   # AVR-GCC firmware
    └── lib/
        └── oled-display/   # Vendored OLED library
```

## License

This project is open source. See [LICENSE](LICENSE) for details.
