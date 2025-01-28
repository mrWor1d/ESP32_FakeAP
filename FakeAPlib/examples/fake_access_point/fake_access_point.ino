/**
 * @file fake_access_point.ino
 * @authors NGUEYOU SIMO, Neil L.; MORENO ROMO, Lucas; RUBIO JIMÉNEZ, Mario
 * @brief Los pines y funciones para leer y escribir en una SD ya están definidos en los archivos fuente SDFiles.
 * 		  Tenga en cuenta que esto solo está destinado a ser utilizado en una placa ESP32 que tiene un lector SD
 * 		  integrado.
 * 		  Las dependencias de esta libreria son:
 * 			- FS.h
 * 			- SD_MMC.h
 *
 * @version 0.1
 * @date 2024-12-29
 *
 */

#include <FakeAPlib.h>

const char *indexPage = "/folder/index.html"; //ruta de la pagina de inicio (del portal cautivo)
const char *ssid	  = "AP_SSID";			  // nombre del punto de acceso
// const char *psw 	  = "AP_PASS";      <------- [opcional] La idea es que sea una wifi publica
const char *WIFI_SSID = "SERVER_WIFI_SSID";   // nombre de la wifi del servidor
const char *WIFI_PSW  = "SERVER_WIFI_PASS";	  // contraseña de la wifi del servidor

IPAddress apip(192, 168, 1, 1);  		//dirección IP de la red del punto de acceso

FakeAP* accessPoint = new FakeAP(8080); // iniciar un servidor el puerto 8080
										// en caso de no indicar ninguno, el puerto por defecto es 80

void setup()
{
	Serial.begin(115200);
	delay(2000);
	Serial.println("Cargando.....");

	//metodo para definir las rutas de las paginas
	//el primer argumento es la ruta y el segundo es el tipo de archivo
	accessPoint->setPath(indexPage, INDEXPAGE);

	//[opcional] configuración del modo de la wifi y del nombre de host
	WiFi.mode(WIFI_AP_STA);
	WiFi.hostname("ESP.accespoint");
	
	//configuración de la ip de la red, el gateway y la mascara de subred
	WiFi.softAPConfig(apip, apip, IPAddress(255, 255, 255, 0));
	//estas configuraciones se tienen que hacer antes de iniciar el punto de acceso

	//iniciar el punto de acceso y el servidor, y conectarse a la wifi inidicada (la del servidor)
	if (!accessPoint->initialize(SSID, "", WIFI_SSID, WIFI_PSW))
    Serial.println("Error en la configuración del punto de acceso");
	//este modo solo inicia el punto de acceso y el servidor si no  se inidica ninguna wifi
	//las contraseñas de las redes wifi son opcionales

  	Serial.println("Configuración acabada!\n
					Acces point on. Listenning....");
}

void loop()
{
	accessPoint->process();
}