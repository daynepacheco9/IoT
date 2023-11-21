#include <Arduino.h>

const uint8_t BUTTON = 13;
const uint8_t LED = 26;

bool pressionado = LOW;
bool last_button_state = LOW;
bool led_state = LOW;
long tempo = 0;

void setup() 
{
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  
}

void loop() 
{
  last_button_state = pressionado;
  pressionado = digitalRead(BUTTON);
  
  if (pressionado &&
      !last_button_state &&
      millis() - tempo > 50)
  {
    led_state = !led_state;
    digitalWrite(LED, led_state);
    tempo = millis();
  }
}
