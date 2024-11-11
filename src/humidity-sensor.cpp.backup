
// DHT11
// adafruit/DHT sensor library@^1.4.6

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>    // подключаем библиотеку для датчика
DHT dht(23, DHT11); // сообщаем на каком порту будет датчик

void setup()
{
    dht.begin();          // запускаем датчик DHT11
    Serial.begin(115200); // подключаем монитор порта
}

void loop()
{
    // считываем температуру (t) и влажность (h)
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // выводим температуру (t) и влажность (h) на монитор порта
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Temperature: ");
    Serial.println(t);
    delay(2000); // задержка 1 секунда
}