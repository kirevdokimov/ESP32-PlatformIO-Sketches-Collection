#include <HardwareSerial.h>

/**
 * Sensor is MH-Z19B
 * sensor rx -> esp32 d17
 * sensor tx -> esp32 d16
 * sensor gnd -> esp32 gnd
 * sensor vcc -> esp32 5v
 *
 * Oled is SSD1306 1.3` 128x64 I2C, Address select 0x78 (which is for some reason means 0x3C)
 * oled scl -> esp32 d22
 * oled sda -> esp32 d21
 * oled gnd -> esp32 gnd
 * oled vcc -> esp32 3.3v or 5v it doesn't matter with my oled
 *
 */

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

#define SENSOR_RX 17
#define SENSOR_TX 16

#define DISPLAY_SCL 22
#define DISPLAY_SDA 21

// #define DEBUG

#define ROTATION_MODE U8G2_R0 // R0 normal, R2 180 degree
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(ROTATION_MODE, U8X8_PIN_NONE, DISPLAY_SCL, DISPLAY_SDA);
void setup(void)
{
    u8g2.begin();
    u8g2.setFont(u8g2_font_spleen32x64_mn);
    u8g2.setCursor(0, 64 - 16);
    u8g2.setContrast(255);

    u8g2.clearBuffer();
    u8g2.setCursor(0, 64 - 16);
    u8g2.print("WAIT");

    u8g2.sendBuffer();

#ifdef DEBUG
    Serial.begin(115200);
#endif

    // To talk to the sensor
    Serial2.begin(9600, SERIAL_8N1, SENSOR_TX, SENSOR_RX);
}

uint8_t readDataMessage[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
String receivedMessage = "";

void loop(void)
{

    // Send message that requests data from the sensor
    Serial2.write(readDataMessage, sizeof(readDataMessage) / sizeof(readDataMessage[0]));

    // Read the response from the sensor
    while (Serial2.available())
    {
        char incomingData = Serial2.read();
        receivedMessage += incomingData;
    }

    if (receivedMessage != "")
    {
#ifdef DEBUG
        Serial.println("Received: ");
        for (int i = 0; i < receivedMessage.length(); i++)
        {
            Serial.print(receivedMessage[i], HEX);
            Serial.print(" ");
        }
        Serial.print("(");
        for (int i = 0; i < receivedMessage.length(); i++)
        {
            Serial.print(receivedMessage[i], DEC);
            Serial.print(" ");
        }
        Serial.print(")");
        Serial.println();
#endif

        if (receivedMessage.length() >= 4)
        {
            uint8_t byte3 = receivedMessage[2];
            uint8_t byte4 = receivedMessage[3];

            int measurement = (byte3 * 256) + byte4;
#ifdef DEBUG
            Serial.println("Measurement: " + String(measurement));
#endif

            u8g2.clearBuffer();
            u8g2.setCursor(0, 64 - 16);
            u8g2.printf("%04d", measurement);

            u8g2.sendBuffer();
        }
        receivedMessage = "";
    }
#ifdef DEBUG
    Serial.println("Take");
#endif
    delay(5000);
}