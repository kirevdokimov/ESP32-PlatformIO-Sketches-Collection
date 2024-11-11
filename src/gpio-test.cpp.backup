#include <Arduino.h>

#define PIN 7

void setup()
{
    pinMode(PIN, OUTPUT);
    Serial.begin(115200);
    Serial.println("Hello World");
}

void loop()
{
    digitalWrite(PIN, HIGH);
    // wait for a second
    delay(1000);
    digitalWrite(PIN, LOW);
    // wait for a second
    delay(1000);
}