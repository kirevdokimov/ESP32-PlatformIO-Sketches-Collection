#include <Arduino.h>
#include <BleGamepad.h>

// PS2 Controller pins
#define PS2_CLK 18  // Clock (с voltage divider 5V->3.3V)
#define PS2_MISO 19 // Data от контроллера (с voltage divider 5V->3.3V)
#define PS2_MOSI 23 // Command к контроллеру (прямое подключение 3.3V)
#define PS2_SS 5    // Attention/Select (прямое подключение 3.3V)

// PS2 команды
#define PS2_CMD_READ_DATA 0x42
#define PS2_CMD_CONFIG_MODE 0x43
#define PS2_CMD_SET_MODE 0x44

// PS2 кнопки (битовые маски для buttons1)
#define PS2_SELECT 0x01
#define PS2_L3 0x02
#define PS2_R3 0x04
#define PS2_START 0x08
#define PS2_UP 0x10
#define PS2_RIGHT 0x20
#define PS2_DOWN 0x40
#define PS2_LEFT 0x80

// PS2 кнопки (битовые маски для buttons2)
#define PS2_L2 0x01
#define PS2_R2 0x02
#define PS2_L1 0x04
#define PS2_R1 0x08
#define PS2_TRIANGLE 0x10
#define PS2_CIRCLE 0x20
#define PS2_X 0x40
#define PS2_SQUARE 0x80

struct PS2Data
{
    uint8_t buttons1;
    uint8_t buttons2;
    uint8_t rightX;
    uint8_t rightY;
    uint8_t leftX;
    uint8_t leftY;
};

PS2Data controllerData;
PS2Data prevControllerData;
BleGamepad bleGamepad("PS2-ESP32-Controller", "Kirill", 100);

void setup()
{
    Serial.begin(115200);
    Serial.println("PS2 to Bluetooth Controller Starting...");

    // Настройка пинов PS2
    pinMode(PS2_SS, OUTPUT);
    pinMode(PS2_CLK, OUTPUT);
    pinMode(PS2_MOSI, OUTPUT);
    pinMode(PS2_MISO, INPUT);

    digitalWrite(PS2_SS, HIGH);
    digitalWrite(PS2_CLK, HIGH);

    // Запуск Bluetooth HID
    Serial.println("Запуск Bluetooth...");
    bleGamepad.begin();

    delay(1000);

    // Инициализация PS2 контроллера
    initPS2Controller();

    Serial.println("Система готова!");
}

void loop()
{
    if (bleGamepad.isConnected())
    {
        if (readPS2Controller())
        {
            updateBluetoothGamepad();
        }
    }
    else
    {
        Serial.println("Ожидание подключения Bluetooth...");
        delay(1000);
    }

    delay(16); // ~60 FPS
}

void initPS2Controller()
{
    Serial.println("Инициализация PS2 контроллера...");

    for (int attempt = 0; attempt < 3; attempt++)
    {
        // Войти в режим конфигурации
        sendPS2Command(PS2_CMD_CONFIG_MODE, 0x01);
        delay(10);

        // Установить аналоговый режим
        sendPS2Command(PS2_CMD_SET_MODE, 0x01);
        delay(10);

        // Выйти из режима конфигурации
        sendPS2Command(PS2_CMD_CONFIG_MODE, 0x00);
        delay(100);

        // Тестовое чтение
        if (readPS2Controller())
        {
            Serial.println("PS2 контроллер успешно инициализирован");
            return;
        }
    }

    Serial.println("Ошибка инициализации PS2 контроллера!");
}

bool readPS2Controller()
{
    digitalWrite(PS2_SS, LOW);
    delayMicroseconds(20);

    uint8_t response[9];
    response[0] = transferPS2Byte(0x01);              // Start
    response[1] = transferPS2Byte(PS2_CMD_READ_DATA); // Command
    response[2] = transferPS2Byte(0x00);              // Padding
    response[3] = transferPS2Byte(0x00);              // Buttons 1
    response[4] = transferPS2Byte(0x00);              // Buttons 2
    response[5] = transferPS2Byte(0x00);              // Right X
    response[6] = transferPS2Byte(0x00);              // Right Y
    response[7] = transferPS2Byte(0x00);              // Left X
    response[8] = transferPS2Byte(0x00);              // Left Y

    digitalWrite(PS2_SS, HIGH);
    delayMicroseconds(20);

    // Проверка валидности ответа
    if (response[1] != 0x41 && response[1] != 0x73)
    {
        return false;
    }

    // Сохранение предыдущих данных
    prevControllerData = controllerData;

    // Сохранение новых данных (инвертируем кнопки)
    controllerData.buttons1 = ~response[3];
    controllerData.buttons2 = ~response[4];
    controllerData.rightX = response[5];
    controllerData.rightY = response[6];
    controllerData.leftX = response[7];
    controllerData.leftY = response[8];

    return true;
}

void sendPS2Command(uint8_t cmd, uint8_t data)
{
    digitalWrite(PS2_SS, LOW);
    delayMicroseconds(20);

    transferPS2Byte(0x01);
    transferPS2Byte(cmd);
    transferPS2Byte(data);

    digitalWrite(PS2_SS, HIGH);
    delayMicroseconds(20);
}

uint8_t transferPS2Byte(uint8_t data)
{
    uint8_t received = 0;

    for (int i = 0; i < 8; i++)
    {
        // Установка бита данных (LSB первый)
        digitalWrite(PS2_MOSI, (data >> i) & 0x01);
        delayMicroseconds(10);

        // Спад тактового сигнала
        digitalWrite(PS2_CLK, LOW);
        delayMicroseconds(10);

        // Чтение бита данных
        if (digitalRead(PS2_MISO))
        {
            received |= (1 << i);
        }

        // Подъем тактового сигнала
        digitalWrite(PS2_CLK, HIGH);
        delayMicroseconds(10);
    }

    return received;
}

void updateBluetoothGamepad()
{
    // Обновление кнопок только при изменении
    updateButton(1, controllerData.buttons2 & PS2_SQUARE, prevControllerData.buttons2 & PS2_SQUARE);     // Square -> Button 1
    updateButton(2, controllerData.buttons2 & PS2_X, prevControllerData.buttons2 & PS2_X);               // X -> Button 2
    updateButton(3, controllerData.buttons2 & PS2_CIRCLE, prevControllerData.buttons2 & PS2_CIRCLE);     // Circle -> Button 3
    updateButton(4, controllerData.buttons2 & PS2_TRIANGLE, prevControllerData.buttons2 & PS2_TRIANGLE); // Triangle -> Button 4

    updateButton(5, controllerData.buttons2 & PS2_L1, prevControllerData.buttons2 & PS2_L1); // L1 -> Button 5
    updateButton(6, controllerData.buttons2 & PS2_R1, prevControllerData.buttons2 & PS2_R1); // R1 -> Button 6
    updateButton(7, controllerData.buttons2 & PS2_L2, prevControllerData.buttons2 & PS2_L2); // L2 -> Button 7
    updateButton(8, controllerData.buttons2 & PS2_R2, prevControllerData.buttons2 & PS2_R2); // R2 -> Button 8

    updateButton(9, controllerData.buttons1 & PS2_SELECT, prevControllerData.buttons1 & PS2_SELECT); // Select -> Button 9
    updateButton(10, controllerData.buttons1 & PS2_START, prevControllerData.buttons1 & PS2_START);  // Start -> Button 10
    updateButton(11, controllerData.buttons1 & PS2_L3, prevControllerData.buttons1 & PS2_L3);        // L3 -> Button 11
    updateButton(12, controllerData.buttons1 & PS2_R3, prevControllerData.buttons1 & PS2_R3);        // R3 -> Button 12

    // D-Pad как HAT
    int16_t hatValue = DPAD_CENTERED;
    if (controllerData.buttons1 & PS2_UP && controllerData.buttons1 & PS2_RIGHT)
        hatValue = DPAD_UP_RIGHT;
    else if (controllerData.buttons1 & PS2_UP && controllerData.buttons1 & PS2_LEFT)
        hatValue = DPAD_UP_LEFT;
    else if (controllerData.buttons1 & PS2_DOWN && controllerData.buttons1 & PS2_RIGHT)
        hatValue = DPAD_DOWN_RIGHT;
    else if (controllerData.buttons1 & PS2_DOWN && controllerData.buttons1 & PS2_LEFT)
        hatValue = DPAD_DOWN_LEFT;
    else if (controllerData.buttons1 & PS2_UP)
        hatValue = DPAD_UP;
    else if (controllerData.buttons1 & PS2_DOWN)
        hatValue = DPAD_DOWN;
    else if (controllerData.buttons1 & PS2_LEFT)
        hatValue = DPAD_LEFT;
    else if (controllerData.buttons1 & PS2_RIGHT)
        hatValue = DPAD_RIGHT;

    bleGamepad.setHat1(hatValue);

    // Аналоговые стики (преобразование 0-255 в -32767 to 32767)
    int16_t leftXAxis = map(controllerData.leftX, 0, 255, -32767, 32767);
    int16_t leftYAxis = map(controllerData.leftY, 0, 255, -32767, 32767);
    int16_t rightXAxis = map(controllerData.rightX, 0, 255, -32767, 32767);
    int16_t rightYAxis = map(controllerData.rightY, 0, 255, -32767, 32767);

    bleGamepad.setLeftThumb(leftXAxis, leftYAxis);
    bleGamepad.setRightThumb(rightXAxis, rightYAxis);

    // Отладочный вывод
    if (controllerData.buttons1 != prevControllerData.buttons1 ||
        controllerData.buttons2 != prevControllerData.buttons2)
    {
        Serial.printf("Buttons: 0x%02X 0x%02X | Sticks: L(%d,%d) R(%d,%d)\n",
                      controllerData.buttons1, controllerData.buttons2,
                      controllerData.leftX, controllerData.leftY,
                      controllerData.rightX, controllerData.rightY);
    }
}

void updateButton(uint8_t buttonNum, bool currentState, bool prevState)
{
    if (currentState != prevState)
    {
        if (currentState)
        {
            bleGamepad.press(buttonNum);
        }
        else
        {
            bleGamepad.release(buttonNum);
        }
    }
}
