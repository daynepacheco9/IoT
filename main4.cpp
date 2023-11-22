#include <Arduino.h>


const uint8_t PIN_INPUT = 26;

const float VREF = 3.3f;
const uint32_t ADC_MAX = 4095;


void setup()
{
  pinMode(PIN_INPUT, INPUT);

  Serial.begin(115200);
}

void loop()
{
  uint16_t input_value = analogRead(PIN_INPUT);
  float voltage = input_value * VREF / ADC_MAX;

  Serial.println(voltage);
}
