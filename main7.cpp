#include <WiFi.h>

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

}

void loop()
{
  int qtde_wifi = WiFi.scanNetworks();

  if (qtde_wifi == 0)
  {
    return;
  }

  for (int i = 0; i < qtde_wifi; i++)
  {
    Serial.printf("Quantidade de redes disponiveis: %i", qtde_wifi);
    Serial.print("Indice: ");
    Serial.print(i+1);
    Serial.print("SSID:");
    Serial.print(WiFi.SSID(i));//nome da rede
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI(i));//intensidade do sinal
    Serial.print('Auth: ');
    Serial.print(WiFi.encryptionType(i));// tipo do wifi, se é aberto ou precida de senha
    Serial.println();// == Serial.print("/n")
  }
  delay(5000);
}
