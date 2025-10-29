#include <Arduino.h>

// Test pins for voltage divider
#define TEST_CLK_PIN 18  // Connect to your voltage divider output
#define TEST_MISO_PIN 19 // Connect to your voltage divider output
#define CONTROL_PIN 23   // We'll use this to generate test signals

void setup()
{
    Serial.begin(115200);
    Serial.println("=== Voltage Divider Test ===");
    Serial.println("This will help verify your voltage divider is working correctly");
    Serial.println();

    // Setup pins
    pinMode(TEST_CLK_PIN, INPUT);
    pinMode(TEST_MISO_PIN, INPUT);
    pinMode(CONTROL_PIN, OUTPUT);

    Serial.println("Pin configuration:");
    Serial.printf("  CLK test pin (GPIO %d): INPUT (connect to voltage divider output)\n", TEST_CLK_PIN);
    Serial.printf("  MISO test pin (GPIO %d): INPUT (connect to voltage divider output)\n", TEST_MISO_PIN);
    Serial.printf("  Control pin (GPIO %d): OUTPUT (generates test signals)\n", CONTROL_PIN);
    Serial.println();

    delay(2000);
}

void loop()
{
    testVoltageReading();
    delay(3000);
}

void testVoltageReading()
{
    Serial.println("=== VOLTAGE DIVIDER TEST ===");
    Serial.println("Testing voltage levels...");

    // Test 1: Generate HIGH signal and measure
    digitalWrite(CONTROL_PIN, HIGH);
    delay(100); // Let voltage settle

    bool clkHigh = digitalRead(TEST_CLK_PIN);
    bool misoHigh = digitalRead(TEST_MISO_PIN);

    Serial.println("Control pin HIGH (3.3V):");
    Serial.printf("  CLK pin reads: %s\n", clkHigh ? "HIGH (good!)" : "LOW (check connection)");
    Serial.printf("  MISO pin reads: %s\n", misoHigh ? "HIGH (good!)" : "LOW (check connection)");

    // Test 2: Generate LOW signal and measure
    digitalWrite(CONTROL_PIN, LOW);
    delay(100); // Let voltage settle

    bool clkLow = digitalRead(TEST_CLK_PIN);
    bool misoLow = digitalRead(TEST_MISO_PIN);

    Serial.println("Control pin LOW (0V):");
    Serial.printf("  CLK pin reads: %s\n", clkLow ? "HIGH (problem!)" : "LOW (good!)");
    Serial.printf("  MISO pin reads: %s\n", misoLow ? "HIGH (problem!)" : "LOW (good!)");

    // Analysis
    Serial.println();
    Serial.println("ANALYSIS:");

    if (clkHigh && !clkLow)
    {
        Serial.println("  ✓ CLK voltage divider working correctly");
    }
    else if (!clkHigh && !clkLow)
    {
        Serial.println("  ✗ CLK always reads LOW - voltage may be too low (<2.0V)");
        Serial.println("    Try reducing R1 or increasing R2 in your voltage divider");
    }
    else if (clkHigh && clkLow)
    {
        Serial.println("  ✗ CLK always reads HIGH - check connections");
    }
    else
    {
        Serial.println("  ✗ CLK reads LOW when should be HIGH - voltage too low");
    }

    if (misoHigh && !misoLow)
    {
        Serial.println("  ✓ MISO voltage divider working correctly");
    }
    else if (!misoHigh && !misoLow)
    {
        Serial.println("  ✗ MISO always reads LOW - voltage may be too low (<2.0V)");
        Serial.println("    Try reducing R1 or increasing R2 in your voltage divider");
    }
    else if (misoHigh && misoLow)
    {
        Serial.println("  ✗ MISO always reads HIGH - check connections");
    }
    else
    {
        Serial.println("  ✗ MISO reads LOW when should be HIGH - voltage too low");
    }

    // Recommendations
    Serial.println();
    if ((clkHigh && !clkLow) && (misoHigh && !misoLow))
    {
        Serial.println("🎉 VOLTAGE DIVIDER IS WORKING CORRECTLY!");
        Serial.println("You can proceed to test PS2 communication.");
    }
    else
    {
        Serial.println("⚠️  VOLTAGE DIVIDER NEEDS ADJUSTMENT:");
        Serial.println("   - Check your resistor connections");
        Serial.println("   - Verify 5V supply voltage");
        Serial.println("   - Measure actual voltages with multimeter");
        Serial.println("   - Current setup should give ~3.0-3.6V for HIGH signals");
    }

    Serial.println();
    Serial.println("Expected voltages:");
    Serial.println("  - 5V input -> ~3.0-3.6V output (reads as HIGH)");
    Serial.println("  - 0V input -> ~0V output (reads as LOW)");
    Serial.println("  - Anything below ~2.0V will read as LOW");
    Serial.println("  - Your measured 3.56V should work perfectly!");

    Serial.println("===========================================");
}
