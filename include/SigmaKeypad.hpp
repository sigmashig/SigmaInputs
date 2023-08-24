#pragma once
#include <Arduino.h>
#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(SIGMA_KEYPAD_EVENT);
#define SIGMA_KEYPAD_SCAN_TIME 50
#define SIGMA_KEYPAD_DEBOUNCE_TIME 100

typedef enum
{
    SIGMA_KEYPAD_EVENT_KEY_PRESSED,
    SIGMA_KEYPAD_EVENT_KEY_RELEASED
} SigmaKeypadEvent;

template <int RowNumber, int ColumnNumber>
class SigmaKeypad
{
public:
    SigmaKeypad(uint pinRows[RowNumber], uint pinColumns[ColumnNumber], const char keys[RowNumber][ColumnNumber]);
    ~SigmaKeypad(){};

    char GetLastKey() { return lastKey; };

    virtual void PinMode(uint pin, uint mode) { pinMode(pin, mode); }
    virtual void DigitalWrite(uint pin, uint value) { digitalWrite(pin, value); }
    virtual int DigitalRead(uint pin) { return digitalRead(pin); }

private:
    //byte keyPadId;
    TimerHandle_t loopTimer;
    StaticTimer_t loopTimerBuffer;
    char lastKey = 0; // confirmed key
    byte lastCol = 0;
    byte lastRow = 0;
    char keys[RowNumber][ColumnNumber];
    uint pinRows[RowNumber];
    uint pinColumns[ColumnNumber];

    char xKey = 0; // key to be confirmed after debounce
    byte xCol = 0;
    byte xRow = 0;
    ulong xTime = 0;
};

template <int RowNumber, int ColumnNumber>
inline SigmaKeypad<RowNumber, ColumnNumber>::SigmaKeypad(uint pinRows[RowNumber], uint pinColumns[ColumnNumber], const char keys[RowNumber][ColumnNumber])
{
    for (int row = 0; row < RowNumber; row++)
    {
        for (int col = 0; col < ColumnNumber; col++)
        {
            this->keys[row][col] = keys[row][col];
        }
    }
    for (int row = 0; row < RowNumber; row++)
    {
        this->pinRows[row] = pinRows[row];
        PinMode(pinRows[row], INPUT_PULLUP);
    }
    for (int col = 0; col < ColumnNumber; col++)
    {
        this->pinColumns[col] = pinColumns[col];
        PinMode(pinColumns[col], OUTPUT);
        DigitalWrite(pinColumns[col], HIGH);
    }

    loopTimer = xTimerCreateStatic(
        "SigmaKeypad", SIGMA_KEYPAD_SCAN_TIME / portTICK_PERIOD_MS, pdTRUE, this,
        [](TimerHandle_t xTimer)
        {
            SigmaKeypad *keypad = static_cast<SigmaKeypad *>(pvTimerGetTimerID(xTimer));
            if (keypad->lastKey != 0)
            {
                keypad->DigitalWrite(keypad->pinColumns[keypad->lastCol], LOW);
                int k = keypad->DigitalRead(keypad->pinRows[keypad->lastRow]);
                keypad->DigitalWrite(keypad->pinColumns[keypad->lastCol], HIGH);
                if (k != LOW)
                {
                    esp_event_post(SIGMA_KEYPAD_EVENT, SIGMA_KEYPAD_EVENT_KEY_RELEASED, &(keypad->lastKey), 1, portMAX_DELAY);
                    keypad->lastKey = 0;
                }
                else
                {
                    return;
                }
            }
            if (keypad->xKey != 0)
            {
                keypad->DigitalWrite(keypad->pinColumns[keypad->xCol], LOW);
                int k = keypad->DigitalRead(keypad->pinRows[keypad->xRow]);
                keypad->DigitalWrite(keypad->pinColumns[keypad->lastCol], HIGH);

                if (k == LOW)
                {
                    if (millis() - keypad->xTime > SIGMA_KEYPAD_DEBOUNCE_TIME)
                    {
                        keypad->lastKey = keypad->xKey;
                        keypad->lastCol = keypad->xCol;
                        keypad->lastRow = keypad->xRow;
                        keypad->xKey = 0;
                        esp_event_post(SIGMA_KEYPAD_EVENT, SIGMA_KEYPAD_EVENT_KEY_PRESSED, &(keypad->lastKey), 1, portMAX_DELAY);
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    keypad->xKey = 0;
                }
            }

            bool keyFound = false;
            for (int col = 0; col < ColumnNumber && !keyFound; col++)
            {
                keypad->DigitalWrite(keypad->pinColumns[col], LOW);
                for (int row = 0; row < RowNumber; row++)
                {
                    if (keypad->DigitalRead(keypad->pinRows[row]) == LOW)
                    {
                        keypad->xKey = keypad->keys[row][col];
                        keypad->xCol = col;
                        keypad->xRow = row;
                        keypad->xTime = millis();
                        keyFound = true;
                        break;
                    }
                }
                keypad->DigitalWrite(keypad->pinColumns[col], HIGH);
            }
        },
        &loopTimerBuffer);
    xTimerStart(loopTimer, 0);
}
