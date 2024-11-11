// // include the library code:
// #include <LiquidCrystal.h>

// // initialize the library with the numbers of the interface pins
// LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

// void setup()
// {
//     // set up the LCD's number of columns and rows:
//     lcd.begin(16, 2);
//     // Print a message to the LCD.
//     lcd.print("circuitschools.");
// }

// void loop()
// {
//     // set the cursor to column 0, line 1
//     // (note: line 1 is the second row, since counting begins with 0):
//     lcd.setCursor(0, 1);
//     // print the number of seconds since reset:
//     lcd.print(1);
// }

/*
 * LAB: 14
 * Name: ESP32 LCD 16x2 Without I2C
 * Author: Khaled Magdy
 * For More Info Visit: www.DeepBlueMbedded.com
 */
#include <Arduino.h>
#include <LiquidCrystal.h>

// Create An LCD Object. Signals: [ RS, EN, D4, D5, D6, D7 ]

LiquidCrystal lcd(2, 8, 3,
                  4, 5, 6, 7);

void setup()
{

    Serial.begin(115200);
    Serial.println("LCD Initialized");
    // Add a delay to allow the LCD to power up
    delay(1000);

    lcd.begin(16, 2);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("123");
}

void loop()
{
}