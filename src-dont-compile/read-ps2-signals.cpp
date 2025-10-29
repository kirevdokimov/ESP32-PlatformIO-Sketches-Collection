// PS2 to Bluetooth Controller - Test Scripts
//
// Choose one of these files to test step by step:
//
// 1. voltage_divider_test.cpp - Test your voltage divider first
// 2. spi_protocol_reader.cpp  - Test SPI communication and PS2 protocol
// 3. ps2_bluetooth_controller.cpp - Full Bluetooth controller functionality

#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    Serial.println("=== PS2 Test Script Selector ===");
    Serial.println();
    Serial.println("Available test scripts:");
    Serial.println("1. voltage_divider_test.cpp - Test voltage levels");
    Serial.println("2. spi_protocol_reader.cpp - Test SPI/PS2 communication");
    Serial.println("3. ps2_bluetooth_controller.cpp - Full controller");
    Serial.println();
    Serial.println("Rename desired script to main.cpp or modify platformio.ini");
}

void loop()
{
    delay(5000);
    Serial.println("Waiting... Please select a test script to run.");
}
