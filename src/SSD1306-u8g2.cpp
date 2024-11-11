
// olikraus/U8g2@^2.36.2

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

#define SCL 22
#define SDA 21

#define ROTATION_MODE U8G2_R0

// _1_ page mode
// _F_ full-buffer mode
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(ROTATION_MODE, U8X8_PIN_NONE, SCL, SDA); // ESP32 Thing, HW I2C with pin remapping

void setup(void)
{
    u8g2.begin();
}

void loop(void)
{
    for (size_t i = 0; i < 100; i++)
    {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_spleen32x64_mn);

        u8g2.setCursor(0, 64 - 16);
        u8g2.print("700");

        u8g2.sendBuffer();
        delay(1000);
    }
}