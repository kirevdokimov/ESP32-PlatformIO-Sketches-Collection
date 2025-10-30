#include <Arduino.h>
#include <SPI.h>

// Arduino Uno Hardware SPI pins (fixed, cannot change)
#define PS2_SS 10 // SPI SS (Chip Select) - PS2 Attention (Pin 10)
// Pin 11 = MOSI (Command to PS2)
// Pin 12 = MISO (Data from PS2)
// Pin 13 = SCK (Clock)

// SPI settings for PS2 protocol
#define PS2_SPI_SPEED 250000   // 250kHz - PS2 controllers are slow
#define PS2_SPI_MODE SPI_MODE3 // CPOL=1, CPHA=1 (idle high, sample on rising edge)
#define PS2_SPI_ORDER LSBFIRST // PS2 uses LSB (Least significant bit) first

void setup()
{
    Serial.begin(115200);
    Serial.println("PS2 SPI Test");

    // Initialize SPI
    SPI.begin();

    // Configure SS pin manually
    pinMode(PS2_SS, OUTPUT);
    digitalWrite(PS2_SS, HIGH);

    delay(3000);
}

void loop()
{
    // Test 1: Check MISO state
    SPI.end();
    pinMode(12, INPUT_PULLUP);
    delay(10);
    bool misoIdle = digitalRead(12);
    SPI.begin();
    Serial.print("MISO idle: ");
    Serial.println(misoIdle ? "HIGH" : "LOW");

    // Test 2: Try PS2 communication
    SPI.beginTransaction(SPISettings(PS2_SPI_SPEED, PS2_SPI_ORDER, PS2_SPI_MODE));

    digitalWrite(PS2_SS, LOW);
    delayMicroseconds(50);

    uint8_t response[3];
    response[0] = SPI.transfer(0x01);
    response[1] = SPI.transfer(0x42);
    response[2] = SPI.transfer(0x00);

    digitalWrite(PS2_SS, HIGH);
    delayMicroseconds(20);

    SPI.endTransaction();

    // Test 3: Check MISO after communication
    SPI.end();
    pinMode(12, INPUT_PULLUP);
    bool misoAfter = digitalRead(12);
    SPI.begin();
    Serial.print("MISO after: ");
    Serial.println(misoAfter ? "HIGH" : "LOW");

    // Print results
    Serial.print("Response: 0x");
    Serial.print(response[0], HEX);
    Serial.print(" 0x");
    Serial.print(response[1], HEX);
    Serial.print(" 0x");
    Serial.println(response[2], HEX);

    if (response[1] == 0x41)
        Serial.println("Digital controller");
    else if (response[1] == 0x73)
        Serial.println("Analog controller");
    else
        Serial.println("No controller");

    delay(2000);
}