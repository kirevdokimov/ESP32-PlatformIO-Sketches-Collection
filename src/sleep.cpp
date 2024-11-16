#include <esp_sleep.h>
#include <Arduino.h>

#define SLEEP_SECOUDNS 5 /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    Serial.begin(115200);
    delay(1000); // Take some time to open up the Serial Monitor

    // Increment boot number and print it every reboot
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));

    /*
    First we configure the wake up source
    We set our ESP32 to wake up every 5 seconds
    */
    esp_sleep_enable_timer_wakeup(SLEEP_SECOUDNS * 1000000);
    Serial.println("Setup ESP32 to sleep for every " + String(SLEEP_SECOUDNS) +
                   " Seconds");

    Serial.println("Going to sleep now");
    delay(1000);
    Serial.flush();
    esp_deep_sleep_start();
    Serial.println("This will never be printed");
}

void loop()
{
    // This is not going to be called
}