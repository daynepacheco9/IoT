#include <Arduino.h>
#include <vector>

template<class T>
using Vector = std::vector<T>;
 
const uint8_t PIN_NTC = 26;

const uint32_t ADC_MAX = (1 << 12) - 1;
const float VREF = 3.3f;
const float R1 = 10000;
 
Vector<float> NTC_10K_temp = { -55, -50, -45, -40, -35,
                               -30, -25, -20, -15, -10,
                               -5, 0, 5, 10, 15,
                               20, 25, 30, 35, 40,
                               45, 50, 55, 60, 65,
                               70, 75, 80, 85, 90,
                               95, 100, 105, 110, 115,
                               120, 125 };
 
Vector<float> NTC_10K_res = { 1214600, 844390, 592430, 419380, 299480,
                              215670, 156410, 114660, 84510, 62927,
                              47077, 35563, 27119, 20860, 16204,
                              12683, 10000, 7942, 6327, 5074,
                              4103, 3336, 2724, 2237, 1846,
                              1530, 1275, 1068, 899.3, 760.7,
                              645.2, 549.4, 470.0, 403.6, 347.4,
                              300.1, 260.1 };

void setup()
{
  pinMode(PIN_NTC, INPUT);
  Serial.begin(115200);
}

void loop()
{
  uint16_t input_value = analogRead(PIN_NTC);
  float voltage = input_value * VREF / ADC_MAX;

  float resistence = R1 * voltage / (VREF - voltage);

  float a = -19.49123972;
  float b = 204.88328885;
  float c = 208.78577114;

  temperature = a * log(resistence - b) +c;


  Serial.println("\nADC: %8i Voltage: %8.2f Resistencia: %8.2f Temperature: %8.2f", input_value, voltage, resistence, temperature);
  delay(1000);

  
}
