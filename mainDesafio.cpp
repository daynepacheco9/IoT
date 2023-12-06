#include "DHT.h"
#include <Arduino.h>
#include <WiFi.h>
#include "FirebaseESP32.h"
#include "PubSubClient.h"
#include "Adafruit_Sensor.h"

#define DHTTYPE DHT11

const uint8_t PIN_DHT = 32;
const uint16_t PIN_LED = 33;
long times = 0;

DHT dht(PIN_DHT, DHTTYPE);
float humidity = 0;
float temperature = 0;

const char WIFI_SSID[] = "Vivo-Internet-BF17";
const char WIFI_PASSWORD[] = "78814222";

const char FIREBASE_HOST[] = "https://iiot-dta-default-rtdb.firebaseio.com";
const char FIREBASE_AUTH[] = "Ag5gJMhAnTWQgDVhegkDRF1uTjJfpMUDkXB8WBEa";

const char* MQTT_BROKER = "teste.mosquitto.org";
const char* MQTT_USERNAME = "";
const char* MQTT_PASSWORD = "";
const int MQTT_PORT = 1883;

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);

FirebaseData fbdo;
FirebaseAuth fbauth;
FirebaseConfig fbconfig;

void processaTudo(float temperature)
{
  if (temperature > 20)
  {
    mqtt_client.publish("iiot-dta/request", "10");
  }
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

void callback(char topic[], byte payload[], uint16_t length)
{
  char transform[length +1];

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  for (uint16_t i = 0; i < length; i++){
    transform[i] = static_cast<char>(payload[i]);
  }
  if (strcmp(topic,"iiot-dta/check") == 0)
  {
    if (strcmp(transform,"100") == 0)
    {
      mqtt_client.publish("iiot-dta/check", "1");
    }
    else if(strcmp(transform, "200") == 0)
    {
      digitalWrite(PIN_LED, 1);
      delay(5000);
      digitalWrite(PIN_LED, 0);
    }
  }
  else if (strcmp(topic,"iiot-dta/request") == 0)
  {
    if(strcmp(transform, "100") == 0)
    {
      if (isnan(humidity) || isnan(temperature))
      {
        Serial.println("Falha na leitura do sensor");
        return;
      }
      processaTudo(temperature);
      bool status;

      FirebaseJson json;
      json.set("/temperature", temperature);
      json.set("/humidity", humidity);
      status = Firebase.updateNode(fbdo, "challenge02/subsys_09", json);

      if (!status){Serial.println(fbdo.errorReason().c_str());}
    }
  }

}


void setup()
{
  dht.begin();
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);

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
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature))
    {
      Serial.println("Falha na leitura do sensor");
      return;

    }
    processaTudo(temperature);
    bool status;

    FirebaseJson json;
    json.set("/temperature", temperature);
    json.set("/humidity", humidity);
    status = Firebase.updateNode(fbdo, "challenge02/subsys_09", json);

    if (!status){Serial.println(fbdo.errorReason().c_str());}
  }
  
}
