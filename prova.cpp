/*--- Includes ---*/
#include "DHT.h"
#include <Arduino.h>
#include <WiFi.h>
#include "FirebaseESP32.h"
#include "PubSubClient.h"
#include "Adafruit_Sensor.h"
#include <vector>
#include <math.h>

template<class T>
using Vector = std::vector<T>;

#define DHTTYPE DHT11


/*--- Variables ---*/
uint16_t input_value = 0;
float voltage = 0;
float resistence = 0;
float temperature_NTC = 0;
float temperature_DHT = 0;

float temp_media = 0;
float temp_dv = 0;
long times = 0;

bool button_state = 0;
bool last_button_state = 0;


/*--- Hardware mapping ---*/
const uint8_t PIN_LED1 = 13;
const uint8_t PIN_LED2 = 12;
const uint8_t PIN_LED3 = 14;
const uint8_t PIN_LED4 = 27;
const uint8_t PIN_NTC = 33;
const uint8_t PIN_DHT= 32 ;
const uint8_t PIN_BUTTON = 25;

DHT dht(PIN_DHT, DHTTYPE);

/*--- Constants ---*/
const uint32_t ADC_MAX = (1 << 12) - 1;
const float VREF = 3.3f;
const float R1 = 10000;

const char WIFI_SSID[] = "Vivo-Internet-BF17";
const char WIFI_PASSWORD[] = "78814222";

const char FIREBASE_HOST[] = "https://iiot-dta-default-rtdb.firebaseio.com";
const char FIREBASE_AUTH[] = "Ag5gJMhAnTWQgDVhegkDRF1uTjJfpMUDkXB8WBEa";

const char* MQTT_BROKER = "test.mosquitto.org";
const char* MQTT_USERNAME = "";
const char* MQTT_PASSWORD = "";
const int MQTT_PORT = 1883;

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);


FirebaseData fbdo;
FirebaseAuth fbauth;
FirebaseConfig fbconfig;

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

/*--- Functions ---*/

float interp(Vector<float> x, Vector<float> y, float t)
{
    uint8_t index = x.size() - 1;
    for (size_t i = 1; i < x.size(); i++)
    {
        if (t > x[i])
        {
            index = i;
            break;
        }
    }

    return y[index] + ((y[index] - y[index - 1]) / (x[index] - x[index - 1]))
                      * (t - x[index]);
}


float media(float temp1, float temp2)
{
    return (temp1 + temp2)/2;
}

float desvioPadrao(float temp1 , float temp2)
{
    return (temp1 - temp2)/(pow(2.0,(1.0/2.0)));
}


void connectWifi(const char ssid[], const char pass[])
{
  WiFi.begin(ssid,pass);

  Serial.print("Connecting");
  while(WiFiClass::status() != WL_CONNECTED)
  {
    Serial.write('.');
    delay(500);
  }
  Serial.print("Connected");
}


void processaTudo()
{
    temperature_DHT = dht.readTemperature();

    input_value = analogRead(PIN_NTC);
    button_state = digitalRead(PIN_BUTTON);

    voltage = static_cast<float>(input_value) * VREF / static_cast<float>(ADC_MAX);

    resistence = R1 * voltage / (VREF - voltage);

    temperature_NTC = interp(NTC_10K_res, NTC_10K_temp, resistence);


    if (!isnan(temperature_DHT))
    {
      Serial.println("Falha na leitura do sensor");
      return;
    }
    
    temp_media = media(temperature_DHT, temperature_NTC);
    
    temp_dv = desvioPadrao(temperature_DHT, temperature_NTC);


    last_button_state = button_state;
    button_state = digitalRead(PIN_BUTTON);

    if (button_state == HIGH && last_button_state == LOW)
    {
        bool status;

        FirebaseJson json;
        json.set("/temp_media", temp_media);
        json.set("/desvio_padrao", temp_dv);
        status = Firebase.updateNode(fbdo, "avaliacao/subsys_09", json);

        digitalWrite(PIN_LED4, 1);

        if (!status){Serial.println(fbdo.errorReason().c_str());}
    }
    digitalWrite(PIN_LED4, 0);
}

void callback(char topic[], byte payload[], uint16_t length)
{
  char transform[length +1];

  for (uint16_t i = 0; i < length; i++){
    transform[i] = static_cast<char>(payload[i]);
  }

  if (strcmp(topic,"avaliacao") == 0)
  {
    if (strcmp(transform,"RTDB") == 0)
    {
      processaTudo();
    }  
  }
}

void setup()
{
    dht.begin();
    Serial.begin(115200);

    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    pinMode(PIN_LED4, OUTPUT);
    pinMode(PIN_NTC, INPUT);
    pinMode(PIN_BUTTON, INPUT);

    connectWifi(WIFI_SSID,WIFI_PASSWORD);

    fbconfig.database_url = FIREBASE_HOST;
    fbconfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    fbdo.setBSSLBufferSize(4096,1024);
    Firebase.reconnectWiFi(true);
    Firebase.begin(&fbconfig, &fbauth);

    mqtt_client.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt_client.setCallback(callback);
    String mqtt_client_id = "esp32-client-" + String(WiFi.macAddress());
    while (!mqtt_client.connected())
    {
        Serial.printf("The client %s connects to the MQTT broker\n",mqtt_client_id.c_str());
        if (mqtt_client.connect(mqtt_client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD))
        {
            Serial.println("MQTT broker connected");
        }
        else
        {
            Serial.print("Failed with state ");
            Serial.print(mqtt_client.state());
            delay(2000);
        }
    }
    mqtt_client.subscribe("iiot-dta/request");
    mqtt_client.subscribe("iiot-dta/check");

}

void loop()
{
  mqtt_client.loop();
  if (millis()-times > 30000)
  {
    times = millis();
    temperature_DHT = dht.readTemperature();

    input_value = analogRead(PIN_NTC);
    button_state = digitalRead(PIN_BUTTON);

    voltage = static_cast<float>(input_value) * VREF / static_cast<float>(ADC_MAX);

    resistence = R1 * voltage / (VREF - voltage);

    temperature_NTC = interp(NTC_10K_res, NTC_10K_temp, resistence);


    if (!isnan(temperature_DHT))
    {
      Serial.println("Falha na leitura do sensor");
      return;
    }
    
    temp_media = media(temperature_DHT, temperature_NTC);
    
    temp_dv = desvioPadrao(temperature_DHT, temperature_NTC);


    last_button_state = button_state;
    button_state = digitalRead(PIN_BUTTON);

    if (button_state == HIGH && last_button_state == LOW)
    {
        bool status;

        FirebaseJson json;
        json.set("/temp media", temp_media);
        json.set("/desvio padrao", temp_dv);
        status = Firebase.updateNode(fbdo, "avaliacao/subsys_09", json);

        digitalWrite(PIN_LED4, 1);

        if (!status){Serial.println(fbdo.errorReason().c_str());}
    }
    digitalWrite(PIN_LED4, 0);

    if (temp_media < 21)
    {
        digitalWrite(PIN_LED1, 0);
        digitalWrite(PIN_LED2, 0);
        digitalWrite(PIN_LED3, 0);
    }
    else if( 21 <= temp_media < 24)
    {
        digitalWrite(PIN_LED1, 1);
        digitalWrite(PIN_LED2, 0);
        digitalWrite(PIN_LED3, 0);
    }
    else if( 24 <= temp_media < 27)
    {
        digitalWrite(PIN_LED1, 1);
        digitalWrite(PIN_LED2, 1);
        digitalWrite(PIN_LED3, 0);
    }
    else if( temp_media >= 27)
    {
        digitalWrite(PIN_LED1, 1);
        digitalWrite(PIN_LED2, 1);
        digitalWrite(PIN_LED3, 1);
    }
  }
  
}
