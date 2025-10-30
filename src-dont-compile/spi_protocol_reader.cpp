#include <Arduino.h>
#include <SPI.h>

// Function declarations
void testVoltageReading();
void testBasicSPI();
void testPS2Communication();
uint8_t transferSPIByte(uint8_t data, bool debug);
void analyzePS2Response(uint8_t *response);
void printBinary(uint8_t value);

// ESP32 Hardware SPI pins (VSPI)
#define PS2_SCK 18  // SPI Clock (SCK) - with voltage divider for PS2
#define PS2_MISO 19 // SPI MISO - from PS2 controller (with voltage divider)
#define PS2_MOSI 23 // SPI MOSI - to PS2 controller (direct 3.3V)
#define PS2_SS 5    // SPI SS (Chip Select) - PS2 Attention (direct 3.3V)

// SPI settings for PS2 protocol
#define PS2_SPI_SPEED 250000   // 250kHz - PS2 controllers are slow
#define PS2_SPI_MODE SPI_MODE3 // CPOL=1, CPHA=1 (idle high, sample on rising edge)
#define PS2_SPI_ORDER LSBFIRST // PS2 uses LSB (Least significant bit) first

// Debug settings
#define DEBUG_TIMING true
#define DEBUG_RAW_BITS true
#define DEBUG_VOLTAGE true

void setup()
{
    Serial.begin(115200);
    Serial.println("=== PS2 SPI Protocol Reader (Hardware SPI) ===");
    Serial.println("Testing voltage divider and SPI communication");
    Serial.println();

    // Initialize SPI
    SPI.begin(PS2_SCK, PS2_MISO, PS2_MOSI, PS2_SS);

    // Configure PS2 SS pin manually (SPI library doesn't control it automatically)
    pinMode(PS2_SS, OUTPUT);
    digitalWrite(PS2_SS, HIGH); // Not selected (PS2 uses active low)

    delay(1000);

    Serial.println("Hardware SPI initialized. Starting tests...");
    Serial.printf("SPI Settings: %d Hz, Mode %d, %s\n",
                  PS2_SPI_SPEED, PS2_SPI_MODE,
                  PS2_SPI_ORDER == LSBFIRST ? "LSB First" : "MSB First");
    Serial.println();
}

void loop()
{

    testBasicSPI();
    delay(1000);

    testPS2Communication();
    delay(2000);

    Serial.println("----------------------------------------");
    delay(3000);
}


void testBasicSPI()
{
    Serial.println("=== BASIC SPI TEST ===");

    // Configure SPI settings for PS2
    SPI.beginTransaction(SPISettings(PS2_SPI_SPEED, PS2_SPI_ORDER, PS2_SPI_MODE));

    // Test SPI transfer without PS2 protocol
    Serial.println("Testing hardware SPI transfer...");

    digitalWrite(PS2_SS, LOW); // Select device
    delayMicroseconds(50);

    uint8_t testByte = 0xAA; // 10101010 pattern
    uint8_t received = SPI.transfer(testByte);

    digitalWrite(PS2_SS, HIGH); // Deselect
    delayMicroseconds(50);

    SPI.endTransaction();

    Serial.printf("Sent: 0x%02X (binary: ", testByte);
    printBinary(testByte);
    Serial.printf("), Received: 0x%02X (binary: ", received);
    printBinary(received);
    Serial.println(")");
    Serial.println();
}

void testPS2Communication()
{
    Serial.println("=== PS2 PROTOCOL TEST ===");

    // Configure SPI settings for PS2
    SPI.beginTransaction(SPISettings(PS2_SPI_SPEED, PS2_SPI_ORDER, PS2_SPI_MODE));

    // Try PS2 handshake
    Serial.println("Attempting PS2 controller communication...");

    digitalWrite(PS2_SS, LOW);
    delayMicroseconds(20);

    Serial.println("Sending PS2 command sequence:");

    // PS2 Read command sequence using hardware SPI
    uint8_t response[6];
    response[0] = transferSPIByte(0x01, true); // Start byte
    response[1] = transferSPIByte(0x42, true); // Read command
    response[2] = transferSPIByte(0x00, true); // Padding
    response[3] = transferSPIByte(0x00, true); // Button data 1
    response[4] = transferSPIByte(0x00, true); // Button data 2
    response[5] = transferSPIByte(0x00, true); // More data

    digitalWrite(PS2_SS, HIGH);
    delayMicroseconds(20);

    SPI.endTransaction();

    // Print results
    Serial.println("Response bytes:");
    for (int i = 0; i < 6; i++)
    {
        Serial.printf("  Byte %d: 0x%02X (", i, response[i]);
        printBinary(response[i]);
        Serial.println(")");
    }

    // Analyze response
    analyzePS2Response(response);
    Serial.println();
}

uint8_t transferSPIByte(uint8_t data, bool debug)
{
    if (debug && DEBUG_RAW_BITS)
    {
        Serial.printf("    Transferring byte 0x%02X: ", data);
        printBinary(data);
    }

    // Use hardware SPI transfer
    uint8_t received = SPI.transfer(data);

    if (debug && DEBUG_RAW_BITS)
    {
        Serial.printf(" -> received 0x%02X (", received);
        printBinary(received);
        Serial.println(")");
    }

    return received;
}

void analyzePS2Response(uint8_t *response)
{
    Serial.println("Analysis:");

    // Check if it looks like a PS2 response
    if (response[1] == 0x41)
    {
        Serial.println("  ✓ Digital controller detected (0x41)");
    }
    else if (response[1] == 0x73)
    {
        Serial.println("  ✓ Analog controller detected (0x73)");
    }
    else if (response[1] == 0xFF)
    {
        Serial.println("  ✗ No controller response (0xFF - likely no controller)");
    }
    else
    {
        Serial.printf("  ? Unknown response ID: 0x%02X\n", response[1]);
    }

    // Check for all zeros (no communication)
    bool allZeros = true;
    bool allOnes = true;
    for (int i = 0; i < 6; i++)
    {
        if (response[i] != 0x00)
            allZeros = false;
        if (response[i] != 0xFF)
            allOnes = false;
    }

    if (allZeros)
    {
        Serial.println("  ✗ All zeros - check wiring and voltage divider");
    }
    else if (allOnes)
    {
        Serial.println("  ✗ All 0xFF - MISO line stuck high, check voltage divider");
    }
    else
    {
        Serial.println("  ✓ Getting some data - communication may be working");
    }

    // Voltage level indication
    if (response[1] == 0x00 || response[1] == 0xFF)
    {
        Serial.println("  💡 Tip: If getting 0x00 or 0xFF, check your voltage divider");
        Serial.println("      Should be ~3.0-3.6V on ESP32 pins");
    }
}

void printBinary(uint8_t value)
{
    for (int i = 7; i >= 0; i--)
    {
        Serial.print((value >> i) & 0x01);
    }
}
