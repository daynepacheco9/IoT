#include <WiFi.h>
#include <Firebase32.h>

const char* WIFI_SSID = "Pacheco"

const char* WIFI_PW = "22092468"

const char* WIFI_HOST = "https://iot-dta-default-rtdb.firebaseio.com/";

const char* FB_KEY = "guiBwX7oeGLuAdoNFF8Jp71rcef4NU23QGyMQUxw";

FirebaseData fbdo;

FirebaseAuth fbauth;

FirebaseConfig fbconfig;

bool connectWIFI(const char* ssid, const char* pw)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  WiFi.begin(WIFI_SSID, WIFI_PW);

  Serial.print("Conectando");
  int tentativas = 0;
  while(WiFi.status() != WL_CONNECTED)
  {
    tentativa++;

    if(tentativa > 300)
    {
      return false;
    }

    Serial.print(".");
    delay(200);

  }
  Serial.println("Conectando com o IP:");

  Serial.println(WiFi.localIP());
  return true;
}

void setup()
{
  Serial.begin(115200);
  connectWIFI(WIFI_SSID, WIFI_PW);

  fbconfig.database_url = FB_HOST;
  fbconfig.signer.tokens.legacy_token = FB_KEY;

  fbdo.setBSSLBufferSize(4096,1024);

  Firebase.reconnectWIFI(true);
  Firebase.begin(&fbconfig, &fbauth);
}

void loop()
{
  float value = 3.14;

  bool status = Firebase.setFloat(fbdo, "/iot-dta/valor", value);

  if(!status)
  {
    Serial.println(fbdo.erroReason().c_str());

  }

  delay(5000);
  

}
