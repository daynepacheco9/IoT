#include <Arduino.h>

const uint8_t PIN_LED = 26;
const uint8_t PIN_BUTTON = 19;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
}

void loop()
{
  bool pressionado = digitalRead(PIN_BUTTON);
  digitalWrite(PIN_LED, pressionado);
}
