#include <Arduino.h>

void setup()
{
    Serial.begin(115200);                      // USB serial to computer
    Serial1.begin(115200, SERIAL_8N1, 16, 17); // Hardware serial to Pi (GPIO9/10/17/18 depending on board)
    Serial.println("ESP32 UART bridge started");
}

void loop()
{
    while (Serial.available())
    {
        Serial1.write(Serial.read());
    }
    while (Serial1.available())
    {
        Serial.write(Serial1.read());
    }
}

// void loop()
// {
//     Serial.println("Sending test");
//     Serial1.println("Hello Pi");
//     delay(1000);

//     while (Serial1.available())
//     {
//         Serial.write(Serial1.read());
//     }
// }