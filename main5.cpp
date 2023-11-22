#include <DHT.h>
#include <Arduino.h>

const uint8_t PIN_INPUT = 26;

#define DHTTYPE DHT11

DHT dht(PIN_INPUT, DHTTYPE);

void setup()
{
  dht.begin();

  Serial.begin(115200);
}

void loop()
{
  float humidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(humidade) || isnan(temperatura))
  {
    Serial.println("Falha na leitura do sensor DHT");
    return;
  }
  Serial.printf("Humidade: %4.2f%% Temperatura: %.2f\n\n", humidade, temperatura);

  delay(2000);

}
