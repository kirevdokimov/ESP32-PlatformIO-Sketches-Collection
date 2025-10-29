

// lib_deps = adafruit/Adafruit SSD1306@^2.5.13

#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
    Serial.begin(115200);
    // Wire.begin(21, 22, 100000); // SDA to GPIO 21, SCL to GPIO 22, 100kHz frequency

    // initialize the OLED object
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    Serial.println("SSD1306 allocation success");
    display.setRotation(2);
    // Clear the buffer.
    display.clearDisplay();

    // Display Text
    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // display.setCursor(0, 28);
    display.println("Hello world!");
    display.display();
}

void loop()
{
    display.clearDisplay();
    display.fillScreen(SSD1306_WHITE);
    display.display();
    delay(1000);

    display.clearDisplay();
    display.println("Hello world!");
    display.display();
    delay(1000);
}